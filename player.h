#ifndef PLAYER_H
#define PLAYER_H

#include "types.h"
#include "game_config.h"

typedef struct {
    float x, y;
    int health;
    Direction lastDirection;
    bool isAttacking;
    float attackTimer;
    float attackCooldown;
    SDL_Rect attackHitbox;
} Player;

void player_init(Player* player);
void player_handle_input(Player* player, bool* keys);
void player_update(Player* player, float dt);
void player_draw(Player* player, SDL_Renderer* renderer);

#endif // PLAYER_H
