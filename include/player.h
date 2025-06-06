#ifndef PLAYER_H
#define PLAYER_H

#include <SDL2/SDL.h> // For SDL_bool
#include "map.h" // Include map.h to get MAP_WIDTH and MAP_HEIGHT

#define PLAYER_MOVE_SPEED 0.03
#define PLAYER_TURN_SPEED 0.02

typedef struct {
    float x;
    float y;
    float angle; // Angle in radians
    int health;
    int stamina;
    int experience;
    int endurance;
    int luck;
} Player;

void init_player(Player *player, float start_x, float start_y, float start_angle);
// void move_player_forward(Player *player, const int map[MAP_HEIGHT][MAP_WIDTH]); // Replaced
// void move_player_backward(Player *player, const int map[MAP_HEIGHT][MAP_WIDTH]); // Replaced
void move_player_with_slide(Player *player, const int map[MAP_HEIGHT][MAP_WIDTH], float move_speed_forward, float move_speed_sideways);
void turn_player_left(Player *player);
void turn_player_right(Player *player);

#endif // PLAYER_H
