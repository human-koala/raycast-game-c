#include "player.h" // Includes map.h via player.h for MAP_HEIGHT, MAP_WIDTH
#include <math.h>   // For sin, cos

void init_player(Player *player, float start_x, float start_y, float start_angle) {
    if (!player) return;
    player->x = start_x;
    player->y = start_y;
    player->angle = start_angle;
    player->health = 100;
    player->stamina = 100;
    player->experience = 0;
    player->endurance = 10;
    player->luck = 5;
}

void move_player_with_slide(Player *player, const int map[MAP_HEIGHT][MAP_WIDTH], float move_speed_forward, float move_speed_sideways) {
    if (!player) return;

    float angle = player->angle;
    float move_x_component = 0.0f;
    float move_y_component = 0.0f;

    // Forward/backward component based on player's angle
    move_x_component += cos(angle) * move_speed_forward;
    move_y_component += sin(angle) * move_speed_forward;

    // Sideways (strafe) component - perpendicular to player's angle
    // move_x_component += cos(angle + M_PI / 2.0f) * move_speed_sideways;
    // move_y_component += sin(angle + M_PI / 2.0f) * move_speed_sideways;
    // Simplified: if strafing, it's often based on a different angle calculation or direct input
    // For this function, we assume move_speed_sideways is passed correctly if strafing is intended.
    // If using A/D for turning, move_speed_sideways will be 0.
    // If A/D are strafe, they'd set move_speed_sideways and call this.
    // Example strafe calculation if needed (ensure M_PI is available if used):
    // float strafe_angle = angle + M_PI / 2.0f; // Angle for strafing right
    // move_x_component += cos(strafe_angle) * move_speed_sideways;
    // move_y_component += sin(strafe_angle) * move_speed_sideways;
    // Since current setup uses A/D for turning, we'll ignore move_speed_sideways for now or assume it's 0.
    // The function signature supports it for future flexibility.

    float target_pos_x = player->x + move_x_component;
    float target_pos_y = player->y + move_y_component;

    // Check collision for X-movement component
    // If moving in X direction, check the map cell at (target_pos_x, player->y)
    if (move_x_component != 0) {
        int map_check_x = (int)(target_pos_x);
        int current_map_y = (int)(player->y);

        if (map_check_x >= 0 && map_check_x < MAP_WIDTH &&
            current_map_y >= 0 && current_map_y < MAP_HEIGHT &&
            map[current_map_y][map_check_x] == 0) {
            player->x = target_pos_x; // Valid move in X
        }
        // Else: Collision in X, player->x is not updated for this component
    }

    // Check collision for Y-movement component
    // If moving in Y direction, check the map cell at (player->x (possibly updated), target_pos_y)
    if (move_y_component != 0) {
        int current_map_x = (int)(player->x); // Use current player X (which might have been updated by X-move)
        int map_check_y = (int)(target_pos_y);

        if (map_check_y >= 0 && map_check_y < MAP_HEIGHT &&
            current_map_x >= 0 && current_map_x < MAP_WIDTH &&
            map[map_check_y][current_map_x] == 0) {
            player->y = target_pos_y; // Valid move in Y
        }
        // Else: Collision in Y, player->y is not updated for this component
    }
}

void turn_player_left(Player *player) {
    if (!player) return;
    player->angle -= PLAYER_TURN_SPEED;
     if (player->angle < 0) player->angle += 2 * M_PI; // Normalize
}

void turn_player_right(Player *player) {
    if (!player) return;
    player->angle += PLAYER_TURN_SPEED;
     if (player->angle > 2 * M_PI) player->angle -= 2 * M_PI; // Normalize
}
