CC = emcc
CFLAGS = -O2 -std=c11 -Wall -Wextra -Iinclude
LDFLAGS = -lSDL2 -lSDL2_image -s USE_SDL=2 -s USE_SDL_IMAGE=2 -s SDL2_IMAGE_FORMATS=\'["png"]\' -s ASYNCIFY

SRC_DIR = src
INCLUDE_DIR = include

SRC_FILES = main.c \
	$(SRC_DIR)/player.c \
	$(SRC_DIR)/particle.c \
	$(SRC_DIR)/enemy.c \
	$(SRC_DIR)/game_map.c \
	$(SRC_DIR)/game.c

OBJ_FILES = $(patsubst %.c, %.o, $(SRC_FILES))

TARGET = index.html

all: $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJ_FILES)
	$(CC) $(CFLAGS) $(OBJ_FILES) -o $@ $(LDFLAGS)

clean:
	rm -f $(OBJ_FILES) $(TARGET)

.PHONY: all clean
