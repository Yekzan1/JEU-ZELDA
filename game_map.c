#include "game_map.h"
#include "world.h"
#include <stdlib.h>

void game_map_init(GameMap* map) {
    int rows = SCREEN_HEIGHT / TILE_SIZE;
    int cols = SCREEN_WIDTH / TILE_SIZE;
    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < cols; x++) {
            if (x == 0 || x == cols - 1 || y == 0 || y == rows - 1) map->tiles[y][x] = 'W';
            else if (rand() % 100 < 5) map->tiles[y][x] = 'T';
            else if ((x == cols / 2 || y == rows / 2) && rand() % 100 < 30) map->tiles[y][x] = 'P';
            else map->tiles[y][x] = 'G';
        }
    }
    map->camera.x = 0;
    map->camera.y = 0;
    map->camera.w = SCREEN_WIDTH;
    map->camera.h = SCREEN_HEIGHT;
}

extern World world;

void game_map_draw(GameMap* map, SDL_Renderer* renderer, SDL_Rect* camera) {
    bool isDark = (world.currentWorld == DARK_WORLD);
    for (int y = 0; y < SCREEN_HEIGHT / TILE_SIZE; y++) {
        for (int x = 0; x < SCREEN_WIDTH / TILE_SIZE; x++) {
            SDL_Rect rect = {x * TILE_SIZE - camera->x, y * TILE_SIZE - camera->y, TILE_SIZE, TILE_SIZE};
            if (!isDark) {
                switch (map->tiles[y][x]) {
                    case 'G': SDL_SetRenderDrawColor(renderer, 100, 180, 80, 255); break; // Grass
                    case 'P': SDL_SetRenderDrawColor(renderer, 200, 180, 120, 255); break; // Path
                    case 'T': SDL_SetRenderDrawColor(renderer, 34, 139, 34, 255); break; // Tree
                    case 'W': SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255); break; // Wall
                }
            } else {
                switch (map->tiles[y][x]) {
                    case 'G': SDL_SetRenderDrawColor(renderer, 60, 40, 100, 255); break; // Dark Grass
                    case 'P': SDL_SetRenderDrawColor(renderer, 80, 60, 120, 255); break; // Dark Path
                    case 'T': SDL_SetRenderDrawColor(renderer, 40, 20, 60, 255); break; // Dark Tree
                    case 'W': SDL_SetRenderDrawColor(renderer, 40, 40, 60, 255); break; // Dark Wall
                }
            }
            SDL_RenderFillRect(renderer, &rect);
        }
    }
}

bool game_map_is_walkable(GameMap* map, int x, int y) {
    int tileX = x / TILE_SIZE;
    int tileY = y / TILE_SIZE;
    if (tileX < 0 || tileX >= SCREEN_WIDTH / TILE_SIZE || tileY < 0 || tileY >= SCREEN_HEIGHT / TILE_SIZE) return false;
    char tile = map->tiles[tileY][tileX];
    return (tile == 'G' || tile == 'P');
}
