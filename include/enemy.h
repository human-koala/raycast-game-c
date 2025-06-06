#ifndef ENEMY_H
#define ENEMY_H

#include <SDL2/SDL.h> // For potential rendering types later
#include "map.h"      // For MAP_WIDTH, MAP_HEIGHT
#include "player.h"   // For Player struct dependency in render_enemies
#include "raycaster.h" // For SCREEN_WIDTH needed by wall_distances array

#define MAX_ENEMIES 1 // For now, only one enemy
#define ENEMY_MOVE_SPEED 0.01

typedef struct {
    float x;
    float y;
    int health;
    SDL_Texture *texture; // For sprite later, NULL for now
    SDL_Color color;      // Simple color for now
    int active;           // Is the enemy active/alive?
    float size;           // Size for rendering (relative to tile size, e.g. 0.5 means half tile height at same distance)
} Enemy;

// Global array for enemies (or pass around)
// extern Enemy enemies[MAX_ENEMIES]; // If defined in enemy.c

void init_enemies(Enemy enemies_list[], int num_enemies, const int map[MAP_HEIGHT][MAP_WIDTH]);
void render_enemies(SDL_Renderer *renderer, Player *player, Enemy enemies_list[], int num_enemies, const int map[MAP_HEIGHT][MAP_WIDTH], float wall_distances[SCREEN_WIDTH]);
void update_single_enemy(Enemy *enemy, Player *player, const int map[MAP_HEIGHT][MAP_WIDTH]);
void update_enemies_ai_logic(Enemy enemies_list[], int num_enemies, Player *player, const int map[MAP_HEIGHT][MAP_WIDTH]);
// wall_distances is the z-buffer from the raycaster, used to ensure enemies are drawn correctly behind walls.

#endif // ENEMY_H
