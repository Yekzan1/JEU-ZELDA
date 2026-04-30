#ifndef TYPES_H
#define TYPES_H

#include <SDL2/SDL.h>
#include <stdbool.h>

typedef enum { UP, DOWN, LEFT, RIGHT } Direction;
typedef enum { PLAYING, GAME_OVER, MENU, PAUSED } GameState;

// Structure pour une hitbox générique
typedef struct {
    float x, y;
    int w, h;
} Hitbox;

#endif // TYPES_H
