#include "raycaster.h"
#include <math.h>
#include <stdlib.h> // Required for rand()

void cast_rays(SDL_Renderer *renderer, Player *player, const int world_map[MAP_HEIGHT][MAP_WIDTH], float wall_distances_out[SCREEN_WIDTH]) {
    SDL_SetRenderDrawColor(renderer, 0x33, 0x33, 0x33, 0xFF); // Ceiling color
    SDL_Rect ceiling_rect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT / 2};
    SDL_RenderFillRect(renderer, &ceiling_rect);

    SDL_SetRenderDrawColor(renderer, 0x66, 0x66, 0x66, 0xFF); // Floor color
    SDL_Rect floor_rect = {0, SCREEN_HEIGHT / 2, SCREEN_WIDTH, SCREEN_HEIGHT / 2};
    SDL_RenderFillRect(renderer, &floor_rect);

    for (int x_col = 0; x_col < SCREEN_WIDTH; x_col++) {
        // Calculate ray position and direction
        // float dir_x = cos(player->angle); // Player's main direction vector component
        // float dir_y = sin(player->angle); // Player's main direction vector component
        // float plane_x = -dir_y * tan(FOV / 2.0); // The x-component of the camera plane vector
        // float plane_y = dir_x * tan(FOV / 2.0);  // The y-component of the camera plane vector
        // More common way to define dir and plane for left-handed coordinate system (SDL default)
        // where positive y is down, positive x is right. Angle 0 is along positive x-axis.
        // For a standard angle (0 = +X, PI/2 = +Y in math coord, but +Y is down in screen coord for SDL rendering)
        // Player angle: 0 means looking right. Increasing angle rotates counter-clockwise in math sense.
        // If player angle 0 is +X, then dirX=1, dirY=0.
        // Camera plane should be perpendicular to direction. For FOV=60 (PI/3), tan(FOV/2)=tan(PI/6)=0.577
        // If dir=(1,0), plane should be (0, +/-0.577) or (0, +/-tan(FOV/2)). Let's use (0, 0.66) for typical FOV.
        // The values from the initial example for plane_x, plane_y are:
        // plane_x = -sin(player->angle) * tan(FOV/2.0); // Corrected from -dir_y
        // plane_y = cos(player->angle) * tan(FOV/2.0);  // Corrected from dir_x
        // Let's use the standard DDA algorithm setup:
        float dir_x = cos(player->angle);
        float dir_y = sin(player->angle);
        // The camera plane is perpendicular to the direction vector.
        // If dir=(dx, dy), plane can be (-dy, dx) or (dy, -dx).
        // Length of plane vector determines FOV. For FOV=60deg, plane length is tan(30deg) * dir_length.
        // Assuming dir_length=1, plane_length=tan(FOV/2).
        // Common setup: plane_x = -dir_y * 0.66; plane_y = dir_x * 0.66; (0.66 is approx tan(33deg) for ~66deg FOV)
        // The provided FOV is M_PI/3 (60 deg), so tan(FOV/2) = tan(M_PI/6) approx 0.577
        float plane_x = -sin(player->angle) * tan(FOV / 2.0);
        float plane_y = cos(player->angle) * tan(FOV / 2.0);


        float camera_x = 2 * x_col / (float)SCREEN_WIDTH - 1; // x-coordinate in camera space
        float ray_dir_x = dir_x + plane_x * camera_x;
        float ray_dir_y = dir_y + plane_y * camera_x;

        int map_x = (int)player->x;
        int map_y = (int)player->y;

        // Length of ray from current position to next x or y-side
        float side_dist_x;
        float side_dist_y;

        // Length of ray from one x or y-side to next x or y-side
        float delta_dist_x = (ray_dir_x == 0) ? 1e30 : fabs(1 / ray_dir_x);
        float delta_dist_y = (ray_dir_y == 0) ? 1e30 : fabs(1 / ray_dir_y);
        float perp_wall_dist;

        // What direction to step in x or y-direction (either +1 or -1)
        int step_x;
        int step_y;

        int hit = 0;  // Was there a wall hit?
        int side;     // Was a NS or a EW wall hit?

        // Calculate step and initial sideDist
        if (ray_dir_x < 0) {
            step_x = -1;
            side_dist_x = (player->x - map_x) * delta_dist_x;
        } else {
            step_x = 1;
            side_dist_x = (map_x + 1.0 - player->x) * delta_dist_x;
        }
        if (ray_dir_y < 0) {
            step_y = -1;
            side_dist_y = (player->y - map_y) * delta_dist_y;
        } else {
            step_y = 1;
            side_dist_y = (map_y + 1.0 - player->y) * delta_dist_y;
        }

        // Perform DDA
        while (hit == 0) {
            // Jump to next map square, OR in x-direction, OR in y-direction
            if (side_dist_x < side_dist_y) {
                side_dist_x += delta_dist_x;
                map_x += step_x;
                side = 0; // Hit an X-side (vertical wall)
            } else {
                side_dist_y += delta_dist_y;
                map_y += step_y;
                side = 1; // Hit a Y-side (horizontal wall)
            }
            // Check if ray has hit a wall or gone out of bounds
            if (map_y < 0 || map_y >= MAP_HEIGHT || map_x < 0 || map_x >= MAP_WIDTH) {
                hit = 1; // Ray out of bounds
                perp_wall_dist = 10000.0; // Assign a large distance
            } else if (world_map[map_y][map_x] > 0) { // Check if ray hits a wall
                hit = 1;
            }
        }

        // Calculate distance projected on camera direction (Euclidean distance would give fisheye effect)
        // Only calculate perp_wall_dist if hit was not due to out of bounds
        if (!(map_y < 0 || map_y >= MAP_HEIGHT || map_x < 0 || map_x >= MAP_WIDTH) && world_map[map_y][map_x] > 0) {
             if (side == 0) { // Hit on X-side
                perp_wall_dist = (map_x - player->x + (1.0 - step_x) / 2.0) / ray_dir_x;
            } else { // Hit on Y-side
                perp_wall_dist = (map_y - player->y + (1.0 - step_y) / 2.0) / ray_dir_y;
            }
        } // If hit was due to out of bounds, perp_wall_dist is already 10000.0 from above.

        // Avoid division by zero or extremely small distances
        if (perp_wall_dist <= 0.01) perp_wall_dist = 0.01;

        // Calculate height of line to draw on screen
        int line_height = (int)(SCREEN_HEIGHT / perp_wall_dist);

        // Calculate lowest and highest pixel to fill in current stripe
        int draw_start = -line_height / 2 + SCREEN_HEIGHT / 2;
        if (draw_start < 0) draw_start = 0;
        int draw_end = line_height / 2 + SCREEN_HEIGHT / 2;
        if (draw_end >= SCREEN_HEIGHT) draw_end = SCREEN_HEIGHT - 1;

        // Choose wall color (example: random color, can be based on wall type or texture)
        // Uint8 r = 50 + rand() % 205;
        // Uint8 g = 50 + rand() % 205;
        // Uint8 b = 50 + rand() % 205;
        Uint8 r,g,b;
        switch(world_map[map_y][map_x]) {
            case 1:  r = 150; g = 0;   b = 0;   break; // Red for wall type 1
            default: r = 100; g = 100; b = 100; break; // Default grey
        }


        // Give different brightness for X and Y sides
        if (side == 1) { // Y-side hit, make it darker
            r = (Uint8)(r * 0.7);
            g = (Uint8)(g * 0.7);
            b = (Uint8)(b * 0.7);
        }
        SDL_SetRenderDrawColor(renderer, r, g, b, 0xFF);

        // Draw the vertical strip
        SDL_RenderDrawLine(renderer, x_col, draw_start, x_col, draw_end);

        // Store the perpendicular wall distance for this column (for z-buffering sprites)
        wall_distances_out[x_col] = perp_wall_dist;
    }
}
