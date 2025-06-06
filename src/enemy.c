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

// Helper functions for distance and Line of Sight

// Note: math.h is already included at the top of this file for sqrtf, fabsf, floorf

float calculate_distance(float x1, float y1, float x2, float y2) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    return sqrtf(dx * dx + dy * dy);
}

// Basic Line of Sight check using grid iteration.
// This is a simple version, can be improved with Bresenham's or similar.
int has_line_of_sight(const int map[MAP_HEIGHT][MAP_WIDTH], float x1, float y1, float x2, float y2) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    float steps;

    if (fabsf(dx) > fabsf(dy)) {
        steps = fabsf(dx);
    } else {
        steps = fabsf(dy);
    }

    if (steps == 0) return 1; // Same point, LOS exists

    float x_increment = dx / steps;
    float y_increment = dy / steps;

    float current_x = x1;
    float current_y = y1;

    for (int i = 0; i < (int)steps; i++) {
        current_x += x_increment;
        current_y += y_increment;

        int map_x = (int)floorf(current_x);
        int map_y = (int)floorf(current_y);

        // Check bounds first
        if (map_x < 0 || map_x >= MAP_WIDTH || map_y < 0 || map_y >= MAP_HEIGHT) {
            return 0; // Line goes out of bounds, effectively blocked
        }
        // Check if the cell is a wall
        if (map[map_y][map_x] != 0) {
            // Check if this wall is not the very start or end tile if they themselves are walls (unlikely for entity positions)
            // For simplicity, any wall along the path blocks LOS.
            return 0; // Wall detected
        }
    }
    return 1; // No wall detected along the line
}

void update_single_enemy(Enemy *enemy, Player *player, const int map[MAP_HEIGHT][MAP_WIDTH]) {
    if (!enemy || !enemy->active || !player) {
        return;
    }

    float distance_to_player = calculate_distance(enemy->x, enemy->y, player->x, player->y);

    // Max distance for enemy to react (e.g. 5 tiles, but line of sight is also checked)
    // Using a smaller aggro distance for initial testing.
    const float max_aggro_distance = 2.0f;
    const float min_approach_distance = 0.5f; // Stop if very close to player

    if (distance_to_player <= max_aggro_distance && distance_to_player > min_approach_distance) {
        if (has_line_of_sight(map, enemy->x, enemy->y, player->x, player->y)) {
            // Enemy sees the player and is close enough, try to move towards player.

            float dir_x = player->x - enemy->x;
            float dir_y = player->y - enemy->y;

            // Normalize direction vector
            float length = sqrtf(dir_x * dir_x + dir_y * dir_y);
            if (length == 0) length = 1; // Avoid division by zero if already at player pos
            float norm_dx = dir_x / length;
            float norm_dy = dir_y / length;

            float next_ex = enemy->x + norm_dx * ENEMY_MOVE_SPEED;
            float next_ey = enemy->y + norm_dy * ENEMY_MOVE_SPEED;

            // Basic wall collision for enemy (similar to player's simplified collision)
            // Store original position for Y-check if X is blocked
            float original_enemy_x = enemy->x;
            // float original_enemy_y = enemy->y; // Not strictly needed with current sliding logic

            // Check X-movement component
            int target_map_x_for_x_move = (int)(next_ex);
            int current_map_y_for_x_check = (int)(enemy->y); // Use current y for X-move check

            if (target_map_x_for_x_move >= 0 && target_map_x_for_x_move < MAP_WIDTH &&
                current_map_y_for_x_check >= 0 && current_map_y_for_x_check < MAP_HEIGHT &&
                map[current_map_y_for_x_check][target_map_x_for_x_move] == 0) {
                enemy->x = next_ex;
            }
            // else: X move blocked, enemy->x remains unchanged.

            // Check Y-movement component
            // Uses potentially updated enemy->x if X move was successful, or original enemy->x if X was blocked.
            // But next_ey was calculated based on original enemy->y.
            int current_map_x_for_y_check = (int)(enemy->x);
            int target_map_y_for_y_move = (int)(next_ey);

            if (target_map_y_for_y_move >= 0 && target_map_y_for_y_move < MAP_HEIGHT &&
                current_map_x_for_y_check >= 0 && current_map_x_for_y_check < MAP_WIDTH &&
                map[target_map_y_for_y_move][current_map_x_for_y_check] == 0) {
                enemy->y = next_ey;
            }
            // else: Y move blocked, enemy->y remains unchanged based on next_ey.
            // If X was successful, Y might still be blocked. If X was blocked, Y is also attempted from original X (effectively).
            // The current logic for Y check is: current_map_x = (int)(enemy->x);
            // This means if X move was successful, Y check is from new X.
            // If X move was blocked, Y check is from old X. This is standard sliding.
        }
    }
    // Else: Player is too far, or no line of sight, or too close. Enemy does nothing (remains idle).
}

void update_enemies_ai_logic(Enemy enemies_list[], int num_enemies, Player *player, const int map[MAP_HEIGHT][MAP_WIDTH]) {
    for (int i = 0; i < num_enemies; i++) {
        if (enemies_list[i].active) {
            update_single_enemy(&enemies_list[i], player, map);
        }
    }
}
