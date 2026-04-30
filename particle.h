#ifndef PARTICLE_H
#define PARTICLE_H

#include <SDL2/SDL.h>
#include "game_config.h"

typedef struct {
    float x, y;
    float vx, vy;
    float life;
    float maxLife;
    SDL_Color color;
} Particle;

void particle_init_system();
void create_particles(float x, float y, SDL_Color color, int count);
void particle_update_all(float dt);
void particle_draw_all(SDL_Renderer* renderer);

#endif // PARTICLE_H
