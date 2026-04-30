#include "enemy.h"
#include "game_config.h"
#include <math.h>

void enemy_init(Enemy* enemy, EnemyType type, float x, float y) {
    enemy->x = x;
    enemy->y = y;
    enemy->startX = x;
    enemy->startY = y;
    enemy->type = type;
    enemy->state = PATROL;
    enemy->direction = (rand() % 4);
    enemy->stateTimer = 2.0f;
    enemy->attackCooldown = 0;
    enemy->alive = true;
    
    switch (type) {
        case HINOX: enemy->health = 10; break;
        case LYNEL: enemy->health = 12; break;
        case TRINEX: enemy->health = 20; break;
        default: enemy->health = 3; break;
    }
}

void enemy_update(Enemy* enemy, float dt, float playerX, float playerY) {
    if (!enemy->alive) return;

    float dx = playerX - enemy->x;
    float dy = playerY - enemy->y;
    float dist = sqrt(dx * dx + dy * dy);

    // Simple State Machine
    enemy->stateTimer -= dt;
    enemy->attackCooldown -= dt;

    if (dist < 150.0f && enemy->state != STUNNED) {
        enemy->state = CHASE;
    } else if (enemy->stateTimer <= 0 && enemy->state != STUNNED) {
        enemy->state = PATROL;
        enemy->stateTimer = 2.0f;
        enemy->direction = rand() % 4;
    }

    float speed = 60.0f;
    if (enemy->type == LYNEL || enemy->type == TEKTITE) speed = 100.0f;

    switch (enemy->state) {
        case PATROL:
            if (enemy->direction == UP) enemy->y -= speed * 0.5f * dt;
            else if (enemy->direction == DOWN) enemy->y += speed * 0.5f * dt;
            else if (enemy->direction == LEFT) enemy->x -= speed * 0.5f * dt;
            else if (enemy->direction == RIGHT) enemy->x += speed * 0.5f * dt;
            break;
        case CHASE:
            if (dist > 10.0f) {
                enemy->x += (dx / dist) * speed * dt;
                enemy->y += (dy / dist) * speed * dt;
            }
            if (dist < 40.0f && enemy->attackCooldown <= 0) {
                enemy->state = ATTACK;
                enemy->attackCooldown = 1.5f;
            }
            break;
        case ATTACK:
            // Attack logic
            if (enemy->attackCooldown < 1.0f) enemy->state = CHASE;
            break;
        case STUNNED:
            if (enemy->stateTimer <= 0) enemy->state = PATROL;
            break;
    }
}

void enemy_draw(Enemy* enemy, SDL_Renderer* renderer) {
    if (!enemy->alive) return;

    SDL_Rect rect = {enemy->x, enemy->y, ENEMY_SIZE, ENEMY_SIZE};
    
    switch (enemy->type) {
        case OCTOROK: SDL_SetRenderDrawColor(renderer, 200, 50, 50, 255); break;
        case MOBLIN: SDL_SetRenderDrawColor(renderer, 150, 75, 0, 255); break;
        case KNIGHT: SDL_SetRenderDrawColor(renderer, 50, 50, 200, 255); break;
        case HINOX: 
            rect.w *= 1.5; rect.h *= 1.5;
            SDL_SetRenderDrawColor(renderer, 100, 100, 50, 255); 
            break;
        case LYNEL: SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); break;
        default: SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255); break;
    }
    
    SDL_RenderFillRect(renderer, &rect);

    // Health bar
    SDL_Rect hb_bg = {enemy->x, enemy->y - 8, ENEMY_SIZE, 3};
    SDL_SetRenderDrawColor(renderer, 50, 0, 0, 255);
    SDL_RenderFillRect(renderer, &hb_bg);
    
    int maxH = 3;
    if (enemy->type == HINOX) maxH = 10;
    if (enemy->type == LYNEL) maxH = 12;
    if (enemy->type == TRINEX) maxH = 20;

    SDL_Rect hb_fg = {enemy->x, enemy->y - 8, (float)enemy->health / maxH * ENEMY_SIZE, 3};
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_RenderFillRect(renderer, &hb_fg);
}
