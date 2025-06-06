#include "map.h"
#include <stdlib.h> // For rand() and srand()
#include <time.h>   // For time() to seed rand()

// If you declared 'map' as extern in map.h, define it here:
// int map[MAP_HEIGHT][MAP_WIDTH];

void init_map(int map[MAP_HEIGHT][MAP_WIDTH]) {
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            map[y][x] = 0; // Initialize all to empty
        }
    }
}

void generate_random_map(int map[MAP_HEIGHT][MAP_WIDTH]) {
    srand(time(NULL)); // Seed the random number generator

    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            // Make borders walls
            if (x == 0 || x == MAP_WIDTH - 1 || y == 0 || y == MAP_HEIGHT - 1) {
                map[y][x] = 1; // Wall
            } else {
                // Randomly place other walls (e.g., 20% chance of a wall)
                if ((rand() % 100) < 20) {
                    map[y][x] = 1; // Wall
                } else {
                    map[y][x] = 0; // Empty space
                }
            }
        }
    }
}
