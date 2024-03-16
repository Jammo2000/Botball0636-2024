#ifndef TOPHAT_H
#define TOPHAT_H
/*
API
 calibrate_tophats() 
 	- follow the prompts on the wombat display to calibrate the line following.
 line_square_up(int maxSpeed) 
 	- drives forward until it hits a line, and then rotates until it is perpendicular to the line
 line_follow_left(int distance, int speed, int side)
 	- 


*/
enum line_follow_side {
    INSIDE = 1,
    OUTSIDE = -1,
};

//define ports

int save_tophat_settings();
int load_tophat_settings();
// Edits the global "tophat_settings"
void calibrate_tophats();

// Speed can't be too high or else it will fail
void line_follow_left(int distance, int speed, enum line_follow_side side);
// Uses the ET sensor, et_distance is just raw ET data
// Will follow until et is more that the distance given for 50 ms
// ET sensor will be ignored until the wheels have traveled min_wheel_distance on average
// 
void line_follow_left_et(int speed, enum line_follow_side side);
void line_follow_right(int distance, int speed, enum line_follow_side side);
void line_follow_right_until_touch(int speed, enum line_follow_side side);


void drive_until_line(int speed);
void line_square_up(int maxSpeed);
#endif