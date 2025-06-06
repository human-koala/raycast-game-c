#ifndef RAYCASTER_H
#define RAYCASTER_H

#include <SDL2/SDL.h>
#include "player.h" // Needs player's position and angle
#include "map.h"    // Needs map data and dimensions

#ifndef M_PI // Define M_PI if not already defined by math.h
#define M_PI 3.14159265358979323846
#endif

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define FOV (M_PI / 3.0) // Field of View (e.g., 60 degrees)

void cast_rays(SDL_Renderer *renderer, Player *player, const int map[MAP_HEIGHT][MAP_WIDTH], float wall_distances_out[SCREEN_WIDTH]);

#endif // RAYCASTER_H
