#include <SDL2/SDL.h>
#include <emscripten.h>
#include "game.h"

Game game;

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
    emscripten_set_main_loop(main_loop, 0, 1);
    game_destroy(&game);
    return 0;
}
