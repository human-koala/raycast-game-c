#include "enemy.h"
#include "player.h"    // For player position in render_enemies
#include "raycaster.h" // For SCREEN_WIDTH, SCREEN_HEIGHT, FOV
#include <stdlib.h>    // For rand
#include <math.h>      // For sqrt, atan2, etc.

// Enemy enemies_list[MAX_ENEMIES]; // Definition if declared extern in .h

void init_enemies(Enemy enemies_list[], int num_enemies, const int map[MAP_HEIGHT][MAP_WIDTH]) {
    for (int i = 0; i < num_enemies; i++) {
        // Find a random empty spot for the enemy, ensuring it's not on a border wall.
        int ex, ey;
        do {
            ex = 1 + rand() % (MAP_WIDTH - 2);  // Avoids border cells [0] and [MAP_WIDTH-1]
            ey = 1 + rand() % (MAP_HEIGHT - 2); // Avoids border cells [0] and [MAP_HEIGHT-1]
        } while (map[ey][ex] != 0); // Ensure it's an empty tile (value 0)

        enemies_list[i].x = ex + 0.5f; // Center of tile
        enemies_list[i].y = ey + 0.5f;
        enemies_list[i].health = 50;
        enemies_list[i].texture = NULL; // No texture yet
        enemies_list[i].color = (SDL_Color){0x00, 0xFF, 0x00, 0xFF}; // Green for zombie
        enemies_list[i].active = 1;
        enemies_list[i].size = 0.5f; // Relative to tile size (for height calculation)
    }
}

// Basic sprite rendering logic (billboarding)
void render_enemies(SDL_Renderer *renderer, Player *player, Enemy enemies_list[], int num_enemies, const int map[MAP_HEIGHT][MAP_WIDTH], float wall_distances[SCREEN_WIDTH]) {
    for (int i = 0; i < num_enemies; i++) {
        if (!enemies_list[i].active) continue;

        // Enemy position relative to player
        float sprite_x = enemies_list[i].x - player->x;
        float sprite_y = enemies_list[i].y - player->y;

        // Player's direction and plane vectors (calculated as in raycaster.c)
        // These definitions must match the ones used in raycaster for consistency
        float dir_x = cos(player->angle);
        float dir_y = sin(player->angle);
        // plane_x = -dir_y * tan(FOV / 2.0) but using player's angle directly:
        float plane_x = -sin(player->angle) * tan(FOV / 2.0); // Perpendicular to player direction
        float plane_y = cos(player->angle) * tan(FOV / 2.0);  // Perpendicular to player direction

        // Transform sprite position to camera space
        // inv_det is the inverse of the determinant of the camera matrix [plane_x dir_x]
        //                                                                [plane_y dir_y]
        float inv_det = 1.0 / (plane_x * dir_y - dir_x * plane_y);

        float transform_x = inv_det * (dir_y * sprite_x - dir_x * sprite_y);
        float transform_y = inv_det * (-plane_y * sprite_x + plane_x * sprite_y); // This is depth inside the screen

        // transform_y is the distance to the sprite. If it's behind the player, don't render.
        if (transform_y <= 0.1) continue; // Clip if behind or too close to player

        // Calculate sprite screen x coordinate
        int sprite_screen_x = (int)((SCREEN_WIDTH / 2) * (1 + transform_x / transform_y));

        // Calculate sprite height and width on screen
        // enemies_list[i].size is a scaling factor for the height.
        // A value of 1.0 would mean the sprite is as tall as a wall at the same distance.
        int sprite_height = abs((int)(SCREEN_HEIGHT / transform_y * enemies_list[i].size));
        int sprite_width = sprite_height; // Assume square sprite for now

        // Calculate draw start and end points for y (vertical)
        int draw_start_y = -sprite_height / 2 + SCREEN_HEIGHT / 2;
        if (draw_start_y < 0) draw_start_y = 0;
        int draw_end_y = sprite_height / 2 + SCREEN_HEIGHT / 2;
        if (draw_end_y >= SCREEN_HEIGHT) draw_end_y = SCREEN_HEIGHT - 1;

        // Calculate draw start and end points for x (horizontal)
        int draw_start_x = -sprite_width / 2 + sprite_screen_x;
        // if (draw_start_x < 0) draw_start_x = 0; // Clipping handled in loop
        int draw_end_x = sprite_width / 2 + sprite_screen_x;
        // if (draw_end_x >= SCREEN_WIDTH) draw_end_x = SCREEN_WIDTH - 1; // Clipping handled in loop

        // Loop through the vertical strips of the sprite
        for (int stripe = draw_start_x; stripe < draw_end_x; stripe++) {
            // Check if this stripe is on screen and in front of a wall (z-buffer check)
            if (stripe >= 0 && stripe < SCREEN_WIDTH && transform_y < wall_distances[stripe]) {
                SDL_SetRenderDrawColor(renderer, enemies_list[i].color.r, enemies_list[i].color.g, enemies_list[i].color.b, enemies_list[i].color.a);
                SDL_RenderDrawLine(renderer, stripe, draw_start_y, stripe, draw_end_y);
            }
        }
    }
}
