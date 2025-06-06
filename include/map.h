#ifndef MAP_H
#define MAP_H

#define MAP_WIDTH 80
#define MAP_HEIGHT 80
#define TILE_SIZE 20 // Example tile size, might be used for rendering later

// Map legend: 0 = empty space, 1 = wall
// extern int map[MAP_HEIGHT][MAP_WIDTH]; // Declare as extern if defined in map.c
                                         // Or define directly if it's simple enough and always static

void generate_random_map(int map[MAP_HEIGHT][MAP_WIDTH]);
void init_map(int map[MAP_HEIGHT][MAP_WIDTH]); // To initialize with empty space first
// (Optional) void load_map_from_file(const char *filename, int map[MAP_HEIGHT][MAP_WIDTH]);

#endif // MAP_H
