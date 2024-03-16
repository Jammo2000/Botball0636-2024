#include "create_move.h"
const int CREATE_CLIFF_THRESHOLD = 2000;
int abs(int x){
    return x >= 0 ? x : -x;
}
int sign(int x){
    if(x>0){
        return 1;
    }else if (x<0){
        return -1;
    }else{
        return 0;
    }
}

int max(int x, int y){
    return x > y ? x : y;
}
int min(int x, int y){
	return x < y ? x : y;
}
void create_drive_until_bump(int speed){
    create_drive_direct(speed, speed);
    while(!get_create_rbump() && !get_create_lbump()) msleep(5);
    create_stop();
}
/// Spin the create
/// Positive speed is counterclockwise
void create_turn_in_place(int degrees, int speed){
    set_create_total_angle(0);
    if(degrees>0){
        create_drive_direct(-speed, speed);
        while(get_create_total_angle()<degrees){
            msleep(30);
        }
    }
    else{
        create_drive_direct(speed, -speed);
        while(get_create_total_angle()>degrees){
            msleep(30);
        }
    }
    create_stop();
}
// Only use for speeds > 100
// and long distances
// Speed is always positive, change distance to go backwards.
void create_drive_accel(int distance, int max_speed, int dist_to_max_speed){
    set_create_distance(0);
    printf("Yeetus");
    int factor = dist_to_max_speed;
    if(distance > 0){
        printf("Deletus");
        while(get_create_distance()<dist_to_max_speed){
            int speed = max_speed*(get_create_distance())/factor;
            speed = max(100, speed);
            printf("%d\n", speed);
            create_drive_direct(speed, speed);
        }
        while(distance-get_create_distance()>dist_to_max_speed){
            msleep(4);
        }
        while(get_create_distance()<distance){
            int speed = max_speed*(distance-get_create_distance())/factor;
            printf("%d\n", speed);
            speed = min(speed+50, max_speed);
            create_drive_direct(speed, speed);
        }
    }else{
    	while(get_create_distance()>-dist_to_max_speed){
            int speed = max_speed*(-get_create_distance())/factor;
            speed = min(-100, speed);
            create_drive_direct(speed, speed);
        }
        while(distance-get_create_distance()>dist_to_max_speed){
            msleep(4);
        }
        while(get_create_distance()<distance){
            int speed = max_speed*(distance+get_create_distance())/factor;
            speed = max(speed-50, max_speed);
            create_drive_direct(speed, speed);
        }
    }
    create_stop();
}
void create_actually_drive_straight(int distance, int speed){
    set_create_total_angle(0);
    set_create_distance(0);
    create_drive_direct(speed,speed);
    if(speed>0){
        while(get_create_distance()<distance){
            int angle_adjust = get_create_total_angle();
            create_drive_direct(speed+angle_adjust,speed-angle_adjust);
            msleep(5);
        }
    }else{
        while(get_create_distance()>distance){
            int angle_adjust = get_create_total_angle();
            create_drive_direct(speed-angle_adjust,speed+angle_adjust);
            msleep(5);
        }
    }
    create_stop();
}

/// Positive angle is counterclockwise
void create_drive_until_angle(int angle, int lspeed, int rspeed){
    set_create_total_angle(0);
    create_drive_direct(lspeed, rspeed);
    while(abs(angle)>abs(get_create_total_angle())) msleep(3);
    create_stop();
}
// Returns a value in [-base_speed, base_speed]
// proportional to the cubed difference between the sensor and the threshold
// Returned value will have the same sign as base_speed if create is over white
int cliff_sensor_to_speed(int sensor, int base_speed){
    int divisor = 800000000/base_speed;
    int diff = sensor-CREATE_CLIFF_THRESHOLD;
    int value = ((long long int)diff * diff * diff)/divisor;
    if(base_speed > 0){
    	return max(min(value, base_speed), -base_speed);
    }else{
    	return min(max(value, base_speed), -base_speed);
    }
}
/// side should be 0 to follow the line on the right
/// or 1 to follow the line on the left
/// DOES NOT WORK BACKWARDS
void create_line_follow_left(int dist, int speed, int side) {
    set_create_distance(0);
    while(abs(get_create_distance()) < abs(dist)) {
        if (side == 0) {
            int sensor = get_create_lfcliff_amt();
            int lspeed = (-cliff_sensor_to_speed(sensor, speed) + speed);
            create_drive_direct(
                lspeed,
                speed
            );
        } else if (side == 1) {
            int sensor = get_create_lfcliff_amt();
            int lspeed = (cliff_sensor_to_speed(sensor, speed) + speed);
            create_drive_direct(
                lspeed,
                speed
            );
        }
    }
    create_stop();
}
void create_line_follow_right(int dist, int speed, int side) {
    set_create_distance(0);
    while(abs(get_create_distance()) < abs(dist)) {
        if (side == 0) {
            int sensor = get_create_rfcliff_amt();
            int rspeed = (cliff_sensor_to_speed(sensor, speed) + speed);
            create_drive_direct(
                speed,
                rspeed
            );
        } else if (side == 1) {
            int sensor = get_create_rfcliff_amt();
            int rspeed = (-cliff_sensor_to_speed(sensor, speed) + speed);
            create_drive_direct(
                speed,
                rspeed
            );
        }
    }
    create_stop();
}
//Might work backwards
// both dist and speed should be negative to go back
// side is same as linefollow left
void create_line_follow_side_right(int dist, int speed, int side) {
    set_create_distance(0);
    while(abs(get_create_distance()) < abs(dist)) {
        if (side == 0) {
            int sensor = get_create_rcliff_amt();
            int rspeed = (cliff_sensor_to_speed(sensor, speed) + speed);
            create_drive_direct(
                speed,
                rspeed
            );
        } else if (side == 1) {
            int sensor = get_create_rcliff_amt();
            int rspeed = (-cliff_sensor_to_speed(sensor, speed) + speed);
            create_drive_direct(
                speed,
                rspeed
            );
        }
    }
    create_stop();
}
void linefollow(float distance, int speed){
    const double slope = 0.000238095238095; // over precise, but idc
    set_create_distance(0);
    while(1){
        if(speed > 0 && get_create_distance()>=distance) break;
        if(speed < 0 && get_create_distance()<=distance) break;
        int left_cliff = get_create_lcliff_amt();
        int right_cliff = get_create_rcliff_amt();
        int left_speed = (slope * (left_cliff - 2600) + 1) * speed;
        int right_speed = (slope * (right_cliff - 2600) + 0.98) * speed;
        display_clear();
        display_printf(0, 0, "Left: %d, Right: %d", left_speed, right_speed);
        create_drive_direct(left_speed, right_speed);
        msleep(5);
    }
    create_stop();
}
void line_follow_until_double_black(int speed){
    const double slope = 0.000238095238095; // over precise, but idc
    while(1){
        int left_cliff = get_create_lcliff_amt();
        int right_cliff = get_create_rcliff_amt();

        if (left_cliff<CREATE_CLIFF_THRESHOLD && right_cliff < CREATE_CLIFF_THRESHOLD) break;

        int left_speed = (slope * (left_cliff - 2600) + 1) * speed;
        int right_speed = (slope * (right_cliff - 2600) + 0.98) * speed;
        display_clear();
        display_printf(0, 0, "Left: %d, Right: %d", left_speed, right_speed);

        create_drive_direct(left_speed, right_speed);
        msleep(5);
    }
    create_stop();
}
// works backwards, just give negative speed
void line_square_up(int speed) {
    create_drive_direct(speed, speed);
    int left = get_create_lfcliff_amt();
    int right = get_create_rfcliff_amt();
    while(left>CREATE_CLIFF_THRESHOLD && right>CREATE_CLIFF_THRESHOLD){
        left = get_create_lfcliff_amt();
        right = get_create_rfcliff_amt();
        msleep(5);
    }
    int slow_speed = 50 * sign(speed);
    int lspeed = cliff_sensor_to_speed(left, slow_speed);
    int rspeed = cliff_sensor_to_speed(right, slow_speed);
    while(abs(rspeed)>4 || abs(lspeed)>4) {
        left = get_create_lfcliff_amt();
        right = get_create_rfcliff_amt();
        lspeed = cliff_sensor_to_speed(left, slow_speed);
        int l = lspeed<=0?min(lspeed, -11):max(lspeed, 11);//Create can't drive at 10 or slower
        rspeed = cliff_sensor_to_speed(right, slow_speed);
        int r = rspeed<=0?min(rspeed, -11):max(rspeed, 11);
        create_drive_direct(l, r);

    }
    create_stop();
}
void line_square_up_side_sensors(int speed) {
    create_drive_direct(speed, speed);
    int left = get_create_lcliff_amt();
    int right = get_create_rcliff_amt();
    // this is to slow down forward motion once we hit the line
    while(left>CREATE_CLIFF_THRESHOLD && right>CREATE_CLIFF_THRESHOLD){
        left = get_create_lcliff_amt();
        right = get_create_rcliff_amt();
        msleep(5);
    }
    int lspeed = cliff_sensor_to_speed(left, 50 * sign(speed));
    int rspeed = cliff_sensor_to_speed(right, 50*sign(speed));
    while(abs(rspeed)>4 || abs(lspeed)>4) {
        left = get_create_lcliff_amt();
        right = get_create_rcliff_amt();
        lspeed = cliff_sensor_to_speed(left, 50*sign(speed));
        int l = lspeed<=0?min(lspeed, -11):max(lspeed, 11);//Create can't drive at 10 or slower
        rspeed = cliff_sensor_to_speed(right, 50*sign(speed));
        int r = rspeed<=0?min(rspeed, -11):max(rspeed, 11);
        create_drive_direct(l, r);

    }
    create_stop();
}