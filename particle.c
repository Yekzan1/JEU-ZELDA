#include "particle.h"
#include <math.h>
#include <stdlib.h>

static Particle particles[MAX_PARTICLES];

void particle_init_system() {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        particles[i].life = 0;
    }
}

void create_particles(float x, float y, SDL_Color color, int count) {
    int created = 0;
    for (int i = 0; i < MAX_PARTICLES && created < count; i++) {
        if (particles[i].life <= 0) {
            float angle = (M_PI * 2 * created) / count;
            particles[i].x = x;
            particles[i].y = y;
            particles[i].vx = cos(angle) * 150;
            particles[i].vy = sin(angle) * 150;
            particles[i].color = color;
            particles[i].life = 0.5f;
            particles[i].maxLife = 0.5f;
            created++;
        }
    }
}

void particle_update_all(float dt) {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (particles[i].life > 0) {
            particles[i].x += particles[i].vx * dt;
            particles[i].y += particles[i].vy * dt;
            particles[i].vy += 100 * dt; // Gravité simple
            particles[i].life -= dt;
        }
    }
}

void particle_draw_all(SDL_Renderer* renderer) {
    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (particles[i].life > 0) {
            SDL_Rect r = {particles[i].x, particles[i].y, 4, 4};
            SDL_SetRenderDrawColor(renderer, particles[i].color.r, particles[i].color.g, particles[i].color.b, (int)(255 * particles[i].life / particles[i].maxLife));
            SDL_RenderFillRect(renderer, &r);
        }
    }
}
