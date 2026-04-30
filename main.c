#include <SDL2/SDL.h>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif
#include "game.h"

Game game;
bool keys[SDL_NUM_SCANCODES] = {0};
World world;

void main_loop() {
    Uint32 currentTicks = SDL_GetTicks();
    float dt = (currentTicks - game.lastTicks) / 1000.0f;
    game.lastTicks = currentTicks;
    if (dt > 0.05f) dt = 0.05f;

    game_handle_input(&game);
    game_update(&game, dt);
    game_draw(&game);
}

int main() {
    game_init(&game);
    #ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(main_loop, 0, 1);
#else
    while (game.gameState != GAME_OVER) {
        main_loop();
        SDL_Delay(16); // ~60 FPS
    }
#endif
    game_destroy(&game);
    return 0;
}
