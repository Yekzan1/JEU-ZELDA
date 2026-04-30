#include "player.h"
#include "game_map.h"
#include "world.h"
#include <math.h>

extern bool keys[SDL_NUM_SCANCODES];
extern World world;

void player_init(Player* player) {
    player->x = SCREEN_WIDTH / 2 - PLAYER_SIZE / 2;
    player->y = SCREEN_HEIGHT / 2 - PLAYER_SIZE / 2;
    player->health = MAX_HEALTH;
    player->maxHealth = MAX_HEALTH;
    player->lastDirection = DOWN;
    player->isAttacking = false;
    player->attackCooldown = 0;
    player->equippedItem = BOMB;
}

void player_handle_input(Player* player, bool* keys) {
    // This could be used for one-time key presses if needed
}

void player_update(Player* player, float dt) {
    float dx = 0, dy = 0;
    if (keys[SDL_SCANCODE_W] || keys[SDL_SCANCODE_UP]) dy -= 1;
    if (keys[SDL_SCANCODE_S] || keys[SDL_SCANCODE_DOWN]) dy += 1;
    if (keys[SDL_SCANCODE_A] || keys[SDL_SCANCODE_LEFT]) dx -= 1;
    if (keys[SDL_SCANCODE_D] || keys[SDL_SCANCODE_RIGHT]) dx += 1;

    if (dx != 0 && dy != 0) {
        float len = sqrt(dx * dx + dy * dy);
        dx /= len; dy /= len;
    }

    if (dx != 0 || dy != 0) {
        if (fabs(dx) > fabs(dy)) player->lastDirection = (dx > 0) ? RIGHT : LEFT;
        else player->lastDirection = (dy > 0) ? DOWN : UP;
    }

    float speed = 200.0f;
    float nextX = player->x + dx * speed * dt;
    float nextY = player->y + dy * speed * dt;

    GameMap* currentMap = world_get_current_map(&world);

    if (game_map_is_walkable(currentMap, nextX, player->y) && 
        game_map_is_walkable(currentMap, nextX + PLAYER_SIZE, player->y) &&
        game_map_is_walkable(currentMap, nextX, player->y + PLAYER_SIZE) &&
        game_map_is_walkable(currentMap, nextX + PLAYER_SIZE, player->y + PLAYER_SIZE)) {
        player->x = nextX;
    }
    
    if (game_map_is_walkable(currentMap, player->x, nextY) && 
        game_map_is_walkable(currentMap, player->x + PLAYER_SIZE, nextY) &&
        game_map_is_walkable(currentMap, player->x, nextY + PLAYER_SIZE) &&
        game_map_is_walkable(currentMap, player->x + PLAYER_SIZE, nextY + PLAYER_SIZE)) {
        player->y = nextY;
    }

    player->attackCooldown -= dt;
    if ((keys[SDL_SCANCODE_SPACE]) && player->attackCooldown <= 0 && !player->isAttacking) {
        player->isAttacking = true;
        player->attackTimer = 0.15f;
        player->attackCooldown = 0.3f;
        switch (player->lastDirection) {
            case RIGHT: player->attackHitbox = (SDL_Rect){player->x + PLAYER_SIZE + 5, player->y + 5, 20, PLAYER_SIZE - 10}; break;
            case LEFT:  player->attackHitbox = (SDL_Rect){player->x - 25, player->y + 5, 20, PLAYER_SIZE - 10}; break;
            case DOWN:  player->attackHitbox = (SDL_Rect){player->x + 5, player->y + PLAYER_SIZE + 5, PLAYER_SIZE - 10, 20}; break;
            case UP:    player->attackHitbox = (SDL_Rect){player->x + 5, player->y - 25, PLAYER_SIZE - 10, 20}; break;
        }
    }

    if (keys[SDL_SCANCODE_Q]) { // Use item
        item_use(player->equippedItem, player->x, player->y, player->lastDirection);
    }

    if (player->isAttacking) {
        player->attackTimer -= dt;
        if (player->attackTimer <= 0) player->isAttacking = false;
    }
}

void player_draw(Player* player, SDL_Renderer* renderer) {
    SDL_Rect pRect = {player->x, player->y, PLAYER_SIZE, PLAYER_SIZE};
    SDL_SetRenderDrawColor(renderer, 100, 150, 200, 255);
    SDL_RenderFillRect(renderer, &pRect);

    if (player->isAttacking) {
        SDL_SetRenderDrawColor(renderer, 255, 200, 0, 150);
        SDL_RenderFillRect(renderer, &player->attackHitbox);
    }
}
