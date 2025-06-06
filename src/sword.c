#include "sword.h"
#include "raycaster.h" // For SCREEN_WIDTH, SCREEN_HEIGHT

void render_sword(SDL_Renderer *renderer) {
    // Define sword properties (position, size, colors)
    // These are screen coordinates, fixed relative to the player's view.

    // Example: A simple vertical sword image slightly to the right of center
    int sword_width = 20;
    int sword_height = 100;
    int sword_screen_x = SCREEN_WIDTH * 0.65; // Positioned to the right-center
    int sword_screen_y = SCREEN_HEIGHT - sword_height - 50; // Positioned towards the bottom

    // Blade (grey)
    SDL_Rect blade_rect = {sword_screen_x, sword_screen_y, sword_width, sword_height};
    SDL_SetRenderDrawColor(renderer, 0xAA, 0xAA, 0xAA, 0xFF); // Light grey
    SDL_RenderFillRect(renderer, &blade_rect);

    // Hilt (brown)
    int hilt_width = sword_width + 10; // Hilt slightly wider than blade
    int hilt_height = 20;
    // Position hilt below the blade
    SDL_Rect hilt_rect = {sword_screen_x - (hilt_width - sword_width)/2, sword_screen_y + sword_height, hilt_width, hilt_height};
    SDL_SetRenderDrawColor(renderer, 0x8B, 0x45, 0x13, 0xFF); // Brown
    SDL_RenderFillRect(renderer, &hilt_rect);

    // Guard (darker grey)
    // Position guard between blade and hilt
    int guard_width = sword_width + 20; // Guard wider than blade
    int guard_height = 10;
    SDL_Rect guard_rect = {sword_screen_x - (guard_width - sword_width)/2, sword_screen_y + sword_height - (guard_height/2) , guard_width, guard_height};
    SDL_SetRenderDrawColor(renderer, 0x66, 0x66, 0x66, 0xFF); // Dark Grey
    SDL_RenderFillRect(renderer, &guard_rect);

    // Add a simple border to make it stand out a bit
    SDL_SetRenderDrawColor(renderer, 0x33, 0x33, 0x33, 0xFF); // Black border
    SDL_RenderDrawRect(renderer, &blade_rect);
    SDL_RenderDrawRect(renderer, &hilt_rect);
    SDL_RenderDrawRect(renderer, &guard_rect);
}
