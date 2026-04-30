#include "world.h"
#include <stdlib.h>

void world_init(World* world) {
    game_map_init(&world->lightMap);
    game_map_init(&world->darkMap);
    
    // Light World: Mostly grass and paths
    for(int y=0; y<SCREEN_HEIGHT/TILE_SIZE; y++) {
        for(int x=0; x<SCREEN_WIDTH/TILE_SIZE; x++) {
            if (x == 0 || x == (SCREEN_WIDTH/TILE_SIZE)-1 || y == 0 || y == (SCREEN_HEIGHT/TILE_SIZE)-1)
                world->lightMap.tiles[y][x] = 'W';
            else if (rand() % 100 < 80) world->lightMap.tiles[y][x] = 'G';
            else world->lightMap.tiles[y][x] = 'P';
        }
    }

    // Dark World: Corrupted, more obstacles, different palette logic in draw
    for(int y=0; y<SCREEN_HEIGHT/TILE_SIZE; y++) {
        for(int x=0; x<SCREEN_WIDTH/TILE_SIZE; x++) {
            if (x == 0 || x == (SCREEN_WIDTH/TILE_SIZE)-1 || y == 0 || y == (SCREEN_HEIGHT/TILE_SIZE)-1)
                world->darkMap.tiles[y][x] = 'W';
            else if (rand() % 100 < 60) world->darkMap.tiles[y][x] = 'W'; // More walls/mountains
            else world->darkMap.tiles[y][x] = 'G';
        }
    }
    
    world->currentWorld = LIGHT_WORLD;
}

void world_transition(World* world) {
    if (world->currentWorld == LIGHT_WORLD) world->currentWorld = DARK_WORLD;
    else world->currentWorld = LIGHT_WORLD;
}

GameMap* world_get_current_map(World* world) {
    return (world->currentWorld == LIGHT_WORLD) ? &world->lightMap : &world->darkMap;
}
