#ifndef HUD_H
#define HUD_H

#include <SDL2/SDL.h>
#include "player.h"

void hud_draw(SDL_Renderer* renderer, Player* player, int rupees, int bombs);

#endif // HUD_H
