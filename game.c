#include "game.h"
extern bool keys[SDL_NUM_SCANCODES];
extern World world;
#include <stdlib.h>
#include <time.h>

void game_init(Game* game) {
    srand(time(NULL));
    SDL_Init(SDL_INIT_VIDEO);
    game->window = SDL_CreateWindow("Zelda-like Engine", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    game->renderer = SDL_CreateRenderer(game->window, -1, SDL_RENDERER_ACCELERATED);

    player_init(&game->player);
    world_init(&world);
    particle_init_system();

    for (int i = 0; i < 5; i++) {
        EnemyType type = (i % 5);
        enemy_init(&game->enemies[i], type, 100 + i * 120, 150);
    }
    for (int i = 5; i < MAX_ENEMIES; i++) game->enemies[i].alive = false;

    game->gameState = PLAYING;
    for (int i = 0; i < SDL_NUM_SCANCODES; i++) keys[i] = false;
    game->lastTicks = SDL_GetTicks();
    game->rupees = 0;
    game->bombs = 5;
    for (int i = 0; i < 7; i++) dungeon_init(&game->dungeons[i], (DungeonType)i);
    quest_init(&game->progression);
}

void game_handle_input(Game* game) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_KEYDOWN) {
            keys[event.key.keysym.scancode] = true;
            if (event.key.keysym.sym == SDLK_m) { // 'M' for Mirror transition
                world_transition(&world);
                create_particles(game->player.x, game->player.y, (SDL_Color){255, 255, 255, 255}, 20);
            }
            if (game->gameState == GAME_OVER && event.key.keysym.sym == SDLK_r) {
                game_init(game);
            }
        } else if (event.type == SDL_KEYUP) {
            keys[event.key.keysym.scancode] = false;
        }
    }
}

void game_update(Game* game, float dt) {
    if (game->gameState != PLAYING) return;

    player_update(&game->player, dt);
    
    // Basic camera follow
    GameMap* currentMap = world_get_current_map(&world);
    currentMap->camera.x = game->player.x - SCREEN_WIDTH / 2;
    currentMap->camera.y = game->player.y - SCREEN_HEIGHT / 2;
    
    // Clamp camera
    if (currentMap->camera.x < 0) currentMap->camera.x = 0;
    if (currentMap->camera.y < 0) currentMap->camera.y = 0;

    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (game->enemies[i].alive) {
            enemy_update(&game->enemies[i], dt, game->player.x, game->player.y);

            SDL_Rect pRect = {game->player.x, game->player.y, PLAYER_SIZE, PLAYER_SIZE};
            SDL_Rect eRect = {game->enemies[i].x, game->enemies[i].y, ENEMY_SIZE, ENEMY_SIZE};

            if (SDL_HasIntersection(&pRect, &eRect) && game->enemies[i].attackCooldown <= 0) {
                game->player.health--;
                game->enemies[i].attackCooldown = 1.0f;
                create_particles(game->player.x + PLAYER_SIZE/2, game->player.y + PLAYER_SIZE/2, (SDL_Color){255, 200, 0, 255}, 8);
            }

            if (game->player.isAttacking && SDL_HasIntersection(&game->player.attackHitbox, &eRect)) {
                game->enemies[i].health--;
                create_particles(game->enemies[i].x + ENEMY_SIZE/2, game->enemies[i].y + ENEMY_SIZE/2, (SDL_Color){255, 100, 100, 255}, 5);
                if (game->enemies[i].health <= 0) {
                    game->enemies[i].alive = false;
                    game->rupees += 5;
                }
            }
        }
    }

    particle_update_all(dt);
    item_update_all(dt);
    for (int i = 0; i < 7; i++) dungeon_update(&game->dungeons[i], game->player.x, game->player.y);
    quest_check_triggers(&game->progression, game->player.x, game->player.y);

    if (game->player.health <= 0) game->gameState = GAME_OVER;
}

void game_draw(Game* game) {
    SDL_SetRenderDrawColor(game->renderer, 26, 26, 46, 255);
    SDL_RenderClear(game->renderer);

    GameMap* currentMap = world_get_current_map(&world);
    game_map_draw(currentMap, game->renderer, &currentMap->camera);

    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (game->enemies[i].alive) {
            // Adjust enemy draw position by camera
            Enemy tempEnemy = game->enemies[i];
            tempEnemy.x -= currentMap->camera.x;
            tempEnemy.y -= currentMap->camera.y;
            enemy_draw(&tempEnemy, game->renderer);
        }
    }

    // Adjust player draw position by camera
    Player tempPlayer = game->player;
    tempPlayer.x -= currentMap->camera.x;
    tempPlayer.y -= currentMap->camera.y;
    // Hitbox also needs adjustment if drawn
    tempPlayer.attackHitbox.x -= currentMap->camera.x;
    tempPlayer.attackHitbox.y -= currentMap->camera.y;
    player_draw(&tempPlayer, game->renderer);
    
    particle_draw_all(game->renderer);
    item_draw_all(game->renderer, &currentMap->camera);
    for (int i = 0; i < 7; i++) dungeon_draw_events(&game->dungeons[i], game->renderer, &currentMap->camera);
    
    hud_draw(game->renderer, &game->player, game->rupees, game->bombs);

    if (game->gameState == GAME_OVER) {
        SDL_SetRenderDrawBlendMode(game->renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(game->renderer, 0, 0, 0, 180);
        SDL_Rect full = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        SDL_RenderFillRect(game->renderer, &full);
    }

    SDL_RenderPresent(game->renderer);
}

void game_destroy(Game* game) {
    SDL_DestroyRenderer(game->renderer);
    SDL_DestroyWindow(game->window);
    SDL_Quit();
}
