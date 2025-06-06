#include <SDL2/SDL.h>
#include <stdio.h>
#include <math.h> // For M_PI and other math functions
#include "player.h"
#include "map.h"
#include "raycaster.h"
#include "enemy.h"
#include "sword.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

int main(int argc, char* argv[]) {
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;

    // Declare game data arrays
    int game_map[MAP_HEIGHT][MAP_WIDTH];
    Enemy enemies[MAX_ENEMIES];
    float wall_distances[SCREEN_WIDTH]; // Z-buffer

    // Initialize map
    init_map(game_map);
    generate_random_map(game_map); // srand is called in here

    // Find a guaranteed empty spot for the player to start
    float start_x = 3.5f; // Default starting position
    float start_y = 3.5f;
    for (int y = 1; y < MAP_HEIGHT - 1; y++) {
        for (int x = 1; x < MAP_WIDTH - 1; x++) {
            if (game_map[y][x] == 0) {
                start_x = x + 0.5f; // Center of the tile
                start_y = y + 0.5f;
                goto found_start_pos; // Exit loops once found
            }
        }
    }
    found_start_pos:;

    // Initialize player
    Player player;
    init_player(&player, start_x, start_y, M_PI / 4.0); // Start angle (e.g. 45 degrees)

    // Initialize enemies
    init_enemies(enemies, MAX_ENEMIES, (const int (*)[MAP_WIDTH])game_map);

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }

    // Create window
    window = SDL_CreateWindow("Raycaster Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Create renderer
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Main loop flag
    int quit = 0;

    // Event handler
    SDL_Event e;

    // Game loop
    while (!quit) {
        // Handle events on queue
        while (SDL_PollEvent(&e) != 0) {
            // User requests quit
            if (e.type == SDL_QUIT) {
                quit = 1;
            }
        }

            // Handle player input
            const Uint8 *keystates = SDL_GetKeyboardState(NULL);
            float current_move_speed = 0.0f;
            // float current_strafe_speed = 0.0f; // Placeholder for future strafing

            if (keystates[SDL_SCANCODE_W]) {
                current_move_speed = PLAYER_MOVE_SPEED;
            }
            if (keystates[SDL_SCANCODE_S]) {
                current_move_speed = -PLAYER_MOVE_SPEED;
            }

            // Turning (A/D or Left/Right arrows)
            if (keystates[SDL_SCANCODE_A] || keystates[SDL_SCANCODE_LEFT]) {
                turn_player_left(&player);
            }
            if (keystates[SDL_SCANCODE_D] || keystates[SDL_SCANCODE_RIGHT]) {
                turn_player_right(&player);
            }

            // Call the new movement function if there's forward/backward speed
            if (current_move_speed != 0.0f) { // || current_strafe_speed != 0.0f) {
                move_player_with_slide(&player, (const int (*)[MAP_WIDTH])game_map, current_move_speed, 0.0f); // 0.0f for sideways speed for now
            }

            // Clear screen (now handled by cast_rays start - floor/ceiling)
            // SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF); // Black
            // SDL_RenderClear(renderer);

            // Render the 3D view (walls) and populate wall_distances
            cast_rays(renderer, &player, (const int (*)[MAP_WIDTH])game_map, wall_distances);

            // Render enemies (sprites)
            render_enemies(renderer, &player, enemies, MAX_ENEMIES, (const int (*)[MAP_WIDTH])game_map, wall_distances);

            render_sword(renderer); // Draw sword on top

        // Update screen
        SDL_RenderPresent(renderer);
    }

    // Destroy window and renderer
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    window = NULL;
    renderer = NULL;

    // Quit SDL subsystems
    SDL_Quit();

    return 0;
}
