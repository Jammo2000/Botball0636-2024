#ifndef CREATE_MOVEMENT_H
#define CREATE_MOVEMENT_H
#include <kipr/wombat.h>
#include <math.h>


int abs(int x);
int sign(int x);

int max(int x, int y);
int min(int x, int y);
extern const int CREATE_CLIFF_THRESHOLD;
void create_drive_until_bump(int speed);
/// Spin the create
/// Positive speed is counterclockwise
void create_turn_in_place(int degrees, int speed);
// Only use for speeds > 100 and long distances
// This accelerates the create up to speed over `dist_to_max_speed`
// Speed is always positive, change distance to go backwards.
void create_drive_accel(int distance, int max_speed, int dist_to_max_speed);
void create_actually_drive_straight(int distance, int speed);

/// Positive angle is counterclockwise
void create_drive_until_angle(int angle, int lspeed, int rspeed);
/// side should be 0 to follow the line on the right
/// or 1 to follow the line on the left
/// DOES NOT WORK BACKWARDS
void create_line_follow_left(int dist, int speed, int side);
void create_line_follow_right(int dist, int speed, int side);
//Might work backwards
// both dist and speed should be negative to go back
// side is same as linefollow left
void create_line_follow_side_right(int dist, int speed, int side);
void linefollow(float distance, int speed);
void line_follow_until_double_black(int speed);
// works backwards, just give negative speed
void line_square_up(int speed);
void line_square_up_side_sensors(int speed);
#endif