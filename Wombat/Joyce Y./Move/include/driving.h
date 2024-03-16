#include <stdbool.h>
//You must call gyro_calibrate() before using this
void drive_straight_gyro(int distance, int speed);

void mrp_fixed(int port, int absvel, int distance);

void drive(int distance, int speed);

void drive_exact(int distance, int speed);

void turn_right(int distance, int speed);

void turn_left(int distance, int speed);

void tank_drive(int left_dist, int right_dist, int speed);

void turn_gyro(int angle, int speed);