#ifndef ENEMY_H
#define ENEMY_H

#include "types.h"
#include "game_config.h"

typedef struct {
    float x, y;
    int health;
    Direction direction;
    float attackCooldown;
    bool alive;
} Enemy;

void enemy_init(Enemy* enemy, float x, float y);
void enemy_update(Enemy* enemy, float dt, float playerX, float playerY);
void enemy_draw(Enemy* enemy, SDL_Renderer* renderer);

#endif // ENEMY_H
