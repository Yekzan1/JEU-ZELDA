#!/bin/bash

mkdir -p assets # Ensure assets directory exists

emcc \
    main.c \
    src/player.c \
    src/particle.c \
    src/enemy.c \
    src/game_map.c \
    src/game.c \
    src/world.c \
    src/hud.c \
    -Iinclude \
    -o index.html \
    -s USE_SDL=2 \
    -s USE_SDL_IMAGE=2 \
    -O2 \
    --preload-file assets
