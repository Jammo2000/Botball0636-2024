#include <kipr/wombat.h>
#include "constants.h"
#include "driving.h"
int copy_sign(int sign, int value){
    if(sign>=0){
        return abs(value);
    }else{
        return -abs(value);
    }
}
void mrp_fixed(int port, int absvel, int distance){
    int start_pos = gmpc(port);
    do{
        mrp(port, absvel, distance);
        msleep(50);
    }while(abs(start_pos-gmpc(port)) < 2);
}
void drive_straight_gyro(int distance, int speed){
    speed = copy_sign(distance, speed);
    float x = 0, y = 0, z = 0;
    double last_time = seconds();
    cmpc(left_wheel);
    cmpc(right_wheel);
    bool slowed = false;
    int distance_traveled;
    while(abs(distance_traveled = (gmpc(left_wheel) +  gmpc(right_wheel))/2) < abs(distance)){
        double t = seconds();
        double dt = t-last_time;
        last_time = t;
        x += gyro_x()*dt;
        y += gyro_y()*dt;
        z += gyro_z()*dt;
        int correction = abs(z)> 0.2 ? 0.01*z*speed : 0;
        if(speed<0) correction = -correction;
        if(abs(speed)>100 && !slowed && abs(distance_traveled-distance)<50){
        	speed = copy_sign(distance, 100);
            slowed = true;
        }
        mav(right_wheel, speed + correction);
        mav(left_wheel, speed - correction);
    }
    mav(right_wheel, 0);
    mav(left_wheel, 0);
}
void turn_gyro(int angle, int speed){
    float x = 0, y = 0, z = 0;
    double last_time = seconds();
    cmpc(left_wheel);
    cmpc(right_wheel);
    mav(right_wheel, -copy_sign(angle, speed));
    mav(left_wheel,  copy_sign(angle, speed));
    bool slowed = false;
    while(abs(z) < abs(angle)){
        double t = seconds();
        double dt = t-last_time;
        last_time = t;
        x += gyro_x()*dt;
        y += gyro_y()*dt;
        z += gyro_z()*dt;
        if(abs(speed)>100 && !slowed && abs(z-angle)<3){
        	mav(right_wheel, -copy_sign(angle, 100));
    		mav(left_wheel,  copy_sign(angle, 100));
            slowed = true;
        }
    }
}
void drive(int distance, int speed) {
    cmpc(left_wheel);
    cmpc(right_wheel);
    mav(left_wheel, speed);
    mav(right_wheel, speed);
    while ((abs(gmpc(left_wheel)) +  abs(gmpc(right_wheel)))/2 < distance) {
        msleep(20);
    }
    // full stop
    mav(left_wheel, 0);
    mav(right_wheel, 0);
    msleep(20);
}
void drive_exact(int distance, int speed){
    cmpc(left_wheel);
    cmpc(right_wheel);
    mav(left_wheel, speed);
    mav(right_wheel, speed);
    bool left_done = false;
    bool right_done = false;
    while (!right_done || !left_done) {
        if (!left_done && gmpc(left_wheel)>=distance){
            mav(left_wheel, 0);
            left_done = true;
        }
        if (!right_done && gmpc(right_wheel)>=distance){
            mav(right_wheel, 0);
            right_done = true;
        }
        msleep(5);
    }
}
void turn_right(int distance, int speed) {
    cmpc(left_wheel); // clear motor postion counter, left wheel move forward in right turn
    mav(left_wheel, speed);
    mav(right_wheel, -speed);
    while (gmpc(left_wheel) < distance) {
        msleep(20);
    }
    // full stop
    mav(left_wheel, 0);
    mav(right_wheel, 0);
}

void turn_left(int distance, int speed) {
    cmpc(right_wheel); // clear motor postion counter, right wheel move forward in left turn
    mav(left_wheel, -speed);
    mav(right_wheel, speed);
    while (gmpc(right_wheel) < distance) {
        msleep(20);
    }
    // full stop
    mav(left_wheel, 0);
    mav(right_wheel, 0);
}

void tank_drive(int left_dist, int right_dist, int speed){
    int left_speed, right_speed;
    if(abs(left_dist)>abs(right_dist)){
        left_speed=copy_sign(left_dist, speed);
        right_speed = copy_sign(right_dist, ((float)right_dist/left_dist)*speed);
    }else{
        right_speed=copy_sign(right_dist, speed);
        left_speed = copy_sign(left_dist, ((float)left_dist/right_dist)*speed);
    }
    mav(left_wheel, left_speed);
    mav(right_wheel, right_speed);
    int left_done = 0;
    int right_done = 0;
    cmpc(left_wheel);
    cmpc(right_wheel);
    while(!left_done || !right_done){
        left_done = abs(gmpc(left_wheel)) >= abs(left_dist);
        if(left_done) off(left_wheel);
        right_done = abs(gmpc(right_wheel)) >= abs(right_dist);
        if(right_done) off(right_wheel);
        msleep(5);
    }
}