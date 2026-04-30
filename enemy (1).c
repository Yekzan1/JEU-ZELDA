#include "enemy.h"
#include "game_config.h"
#include <math.h>

void enemy_init(Enemy* enemy, float x, float y) {
    enemy->x = x;
    enemy->y = y;
    enemy->health = ENEMY_HEALTH;
    enemy->alive = true;
    enemy->attackCooldown = 0;
    enemy->direction = DOWN; // Default direction
}

void enemy_update(Enemy* enemy, float dt, float playerX, float playerY) {
    if (!enemy->alive) return;

    // Simple AI: pursue player
    float ex = playerX - enemy->x;
    float ey = playerY - enemy->y;
    float dist = sqrt(ex * ex + ey * ey);

    if (dist > 0) {
        enemy->x += (ex / dist) * 80 * dt;
        enemy->y += (ey / dist) * 80 * dt;
    }

    enemy->attackCooldown -= dt;
}

void enemy_draw(Enemy* enemy, SDL_Renderer* renderer) {
    if (!enemy->alive) return;

    SDL_Rect rect = {enemy->x, enemy->y, ENEMY_SIZE, ENEMY_SIZE};
    SDL_SetRenderDrawColor(renderer, 200, 50, 50, 255);
    SDL_RenderFillRect(renderer, &rect);

    // Health bar
    SDL_Rect hb_bg = {enemy->x, enemy->y - 8, ENEMY_SIZE, 3};
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &hb_bg);
    
    SDL_Rect hb_fg = {enemy->x, enemy->y - 8, (float)enemy->health / ENEMY_HEALTH * ENEMY_SIZE, 3};
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderFillRect(renderer, &hb_fg);
}
