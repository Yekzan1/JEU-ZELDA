#ifndef GAME_MAP_H
#define GAME_MAP_H

#include "game_config.h"
#include <SDL2/SDL.h>

typedef struct {
    char tiles[SCREEN_HEIGHT / TILE_SIZE][SCREEN_WIDTH / TILE_SIZE];
    SDL_Rect camera;
} GameMap;

void game_map_init(GameMap* map);
void game_map_draw(GameMap* map, SDL_Renderer* renderer, SDL_Rect* camera);
bool game_map_is_walkable(GameMap* map, int x, int y);

#endif // GAME_MAP_H
