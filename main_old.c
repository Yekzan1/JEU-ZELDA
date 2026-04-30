#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <emscripten.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define TILE_SIZE 32
#define PLAYER_SIZE 32
#define ENEMY_SIZE 28
#define MAX_PARTICLES 100
#define MAX_ENEMIES 10
#define MAX_HEALTH 6
#define ENEMY_HEALTH 3

typedef enum { UP, DOWN, LEFT, RIGHT } Direction;
typedef enum { PLAYING, GAME_OVER } GameState;

typedef struct {
    float x, y;
    float vx, vy;
    float life;
    float maxLife;
    SDL_Color color;
} Particle;

typedef struct {
    float x, y;
    int health;
    Direction direction;
    float attackCooldown;
    bool alive;
} Enemy;

typedef struct {
    float x, y;
    int health;
    Direction lastDirection;
    bool isAttacking;
    float attackTimer;
    float attackCooldown;
    SDL_Rect attackHitbox;
} Player;

typedef struct {
    char tiles[SCREEN_HEIGHT / TILE_SIZE][SCREEN_WIDTH / TILE_SIZE];
} GameMap;

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
Player player;
Enemy enemies[MAX_ENEMIES];
Particle particles[MAX_PARTICLES];
GameMap gameMap;
GameState gameState = PLAYING;
bool keys[SDL_NUM_SCANCODES] = {0};
Uint32 lastTicks = 0;

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

bool check_collision(SDL_Rect a, SDL_Rect b) {
    return SDL_HasIntersection(&a, &b);
}

void init_game() {
    player.x = SCREEN_WIDTH / 2 - PLAYER_SIZE / 2;
    player.y = SCREEN_HEIGHT / 2 - PLAYER_SIZE / 2;
    player.health = MAX_HEALTH;
    player.lastDirection = DOWN;
    player.isAttacking = false;
    player.attackCooldown = 0;

    int rows = SCREEN_HEIGHT / TILE_SIZE;
    int cols = SCREEN_WIDTH / TILE_SIZE;
    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < cols; x++) {
            if (x == 0 || x == cols - 1 || y == 0 || y == rows - 1) gameMap.tiles[y][x] = 'W';
            else if (rand() % 100 < 5) gameMap.tiles[y][x] = 'T';
            else if ((x == cols / 2 || y == rows / 2) && rand() % 100 < 30) gameMap.tiles[y][x] = 'P';
            else gameMap.tiles[y][x] = 'G';
        }
    }

    for (int i = 0; i < 3; i++) {
        enemies[i].x = (i == 0) ? 150 : (i == 1) ? 600 : 150;
        enemies[i].y = (i == 0) ? 150 : (i == 1) ? 150 : 500;
        enemies[i].health = ENEMY_HEALTH;
        enemies[i].alive = true;
        enemies[i].attackCooldown = 0;
    }
    for (int i = 3; i < MAX_ENEMIES; i++) enemies[i].alive = false;
    for (int i = 0; i < MAX_PARTICLES; i++) particles[i].life = 0;
    
    gameState = PLAYING;
}

void handle_input() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_KEYDOWN) {
            keys[event.key.keysym.scancode] = true;
            if (gameState == GAME_OVER && event.key.keysym.sym == SDLK_r) init_game();
        } else if (event.type == SDL_KEYUP) {
            keys[event.key.keysym.scancode] = false;
        }
    }
}

void update() {
    Uint32 currentTicks = SDL_GetTicks();
    float dt = (currentTicks - lastTicks) / 1000.0f;
    lastTicks = currentTicks;
    if (dt > 0.05f) dt = 0.05f;

    if (gameState != PLAYING) return;

    float dx = 0, dy = 0;
    if (keys[SDL_SCANCODE_W] || keys[SDL_SCANCODE_UP]) dy -= 1;
    if (keys[SDL_SCANCODE_S] || keys[SDL_SCANCODE_DOWN]) dy += 1;
    if (keys[SDL_SCANCODE_A] || keys[SDL_SCANCODE_LEFT]) dx -= 1;
    if (keys[SDL_SCANCODE_D] || keys[SDL_SCANCODE_RIGHT]) dx += 1;

    if (dx != 0 && dy != 0) {
        float len = sqrt(dx * dx + dy * dy);
        dx /= len; dy /= len;
    }

    if (dx != 0 || dy != 0) {
        if (fabs(dx) > fabs(dy)) player.lastDirection = (dx > 0) ? RIGHT : LEFT;
        else player.lastDirection = (dy > 0) ? DOWN : UP;
    }

    float nextX = player.x + dx * 200 * dt;
    float nextY = player.y + dy * 200 * dt;

    if (nextX >= TILE_SIZE && nextX + PLAYER_SIZE <= SCREEN_WIDTH - TILE_SIZE) player.x = nextX;
    if (nextY >= TILE_SIZE && nextY + PLAYER_SIZE <= SCREEN_HEIGHT - TILE_SIZE) player.y = nextY;

    player.attackCooldown -= dt;
    if ((keys[SDL_SCANCODE_E] || keys[SDL_SCANCODE_SPACE]) && player.attackCooldown <= 0 && !player.isAttacking) {
        player.isAttacking = true;
        player.attackTimer = 0.15f;
        player.attackCooldown = 0.3f;
        int offset = PLAYER_SIZE / 2 + 5;
        switch (player.lastDirection) {
            case RIGHT: player.attackHitbox = (SDL_Rect){player.x + PLAYER_SIZE + 5, player.y + 5, 20, PLAYER_SIZE - 10}; break;
            case LEFT:  player.attackHitbox = (SDL_Rect){player.x - 25, player.y + 5, 20, PLAYER_SIZE - 10}; break;
            case DOWN:  player.attackHitbox = (SDL_Rect){player.x + 5, player.y + PLAYER_SIZE + 5, PLAYER_SIZE - 10, 20}; break;
            case UP:    player.attackHitbox = (SDL_Rect){player.x + 5, player.y - 25, PLAYER_SIZE - 10, 20}; break;
        }
    }

    if (player.isAttacking) {
        player.attackTimer -= dt;
        if (player.attackTimer <= 0) player.isAttacking = false;
    }

    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!enemies[i].alive) continue;
        float ex = player.x - enemies[i].x;
        float ey = player.y - enemies[i].y;
        float dist = sqrt(ex * ex + ey * ey);
        if (dist > 0) {
            enemies[i].x += (ex / dist) * 80 * dt;
            enemies[i].y += (ey / dist) * 80 * dt;
        }
        enemies[i].attackCooldown -= dt;
        SDL_Rect pRect = {player.x, player.y, PLAYER_SIZE, PLAYER_SIZE};
        SDL_Rect eRect = {enemies[i].x, enemies[i].y, ENEMY_SIZE, ENEMY_SIZE};
        if (check_collision(pRect, eRect) && enemies[i].attackCooldown <= 0) {
            player.health--;
            enemies[i].attackCooldown = 1.0f;
            create_particles(player.x + PLAYER_SIZE/2, player.y + PLAYER_SIZE/2, (SDL_Color){255, 200, 0, 255}, 8);
        }
        if (player.isAttacking && check_collision(player.attackHitbox, eRect)) {
            enemies[i].health--;
            create_particles(enemies[i].x + ENEMY_SIZE/2, enemies[i].y + ENEMY_SIZE/2, (SDL_Color){255, 100, 100, 255}, 5);
            if (enemies[i].health <= 0) enemies[i].alive = false;
        }
    }

    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (particles[i].life > 0) {
            particles[i].x += particles[i].vx * dt;
            particles[i].y += particles[i].vy * dt;
            particles[i].vy += 100 * dt;
            particles[i].life -= dt;
        }
    }

    if (player.health <= 0) gameState = GAME_OVER;
}

void draw() {
    SDL_SetRenderDrawColor(renderer, 26, 26, 46, 255);
    SDL_RenderClear(renderer);

    for (int y = 0; y < SCREEN_HEIGHT / TILE_SIZE; y++) {
        for (int x = 0; x < SCREEN_WIDTH / TILE_SIZE; x++) {
            SDL_Rect rect = {x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE};
            switch (gameMap.tiles[y][x]) {
                case 'G': SDL_SetRenderDrawColor(renderer, 100, 180, 80, 255); break;
                case 'P': SDL_SetRenderDrawColor(renderer, 200, 180, 120, 255); break;
                case 'T': SDL_SetRenderDrawColor(renderer, 34, 139, 34, 255); break;
                case 'W': SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255); break;
            }
            SDL_RenderFillRect(renderer, &rect);
        }
    }

    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i].alive) {
            SDL_Rect rect = {enemies[i].x, enemies[i].y, ENEMY_SIZE, ENEMY_SIZE};
            SDL_SetRenderDrawColor(renderer, 200, 50, 50, 255);
            SDL_RenderFillRect(renderer, &rect);
            SDL_Rect hb = {enemies[i].x, enemies[i].y - 8, ENEMY_SIZE, 3};
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            SDL_RenderFillRect(renderer, &hb);
            hb.w = (float)enemies[i].health / ENEMY_HEALTH * ENEMY_SIZE;
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
            SDL_RenderFillRect(renderer, &hb);
        }
    }

    SDL_Rect pRect = {player.x, player.y, PLAYER_SIZE, PLAYER_SIZE};
    SDL_SetRenderDrawColor(renderer, 100, 150, 200, 255);
    SDL_RenderFillRect(renderer, &pRect);

    if (player.isAttacking) {
        SDL_SetRenderDrawColor(renderer, 255, 200, 0, 150);
        SDL_RenderFillRect(renderer, &player.attackHitbox);
    }

    for (int i = 0; i < MAX_PARTICLES; i++) {
        if (particles[i].life > 0) {
            SDL_Rect r = {particles[i].x, particles[i].y, 4, 4};
            SDL_SetRenderDrawColor(renderer, particles[i].color.r, particles[i].color.g, particles[i].color.b, (int)(255 * particles[i].life / particles[i].maxLife));
            SDL_RenderFillRect(renderer, &r);
        }
    }

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    for (int i = 0; i < player.health; i++) {
        SDL_Rect h = {20 + i * 20, 20, 15, 15};
        SDL_RenderFillRect(renderer, &h);
    }

    if (gameState == GAME_OVER) {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 180);
        SDL_Rect full = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
        SDL_RenderFillRect(renderer, &full);
    }

    SDL_RenderPresent(renderer);
}

void main_loop() {
    handle_input();
    update();
    draw();
}

int main() {
    srand(time(NULL));
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("Zelda-like MVP", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    init_game();
    emscripten_set_main_loop(main_loop, 0, 1);
    return 0;
}
