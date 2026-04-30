#include "hud.h"
#include "game_config.h"

void hud_draw(SDL_Renderer* renderer, Player* player, int rupees, int bombs) {
    // Draw hearts
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    for (int i = 0; i < player->health; i++) {
        SDL_Rect h = {20 + i * 20, 20, 15, 15};
        SDL_RenderFillRect(renderer, &h);
    }

    // Draw Rupees (simple text-like representation with rectangles)
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_Rect rRect = {20, 45, 10, 15};
    SDL_RenderFillRect(renderer, &rRect);
    // (Rupee count display logic would go here)

    // Draw Bombs
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
    SDL_Rect bRect = {20, 70, 15, 15};
    SDL_RenderFillRect(renderer, &bRect);
}
