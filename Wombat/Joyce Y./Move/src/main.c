#include <kipr/wombat.h>
#include <constants.h>
#include <tophat.h>
#include "driving.h"

int main()
{
    gyro_calibrate();
    enable_servos();
    calibrate_tophats();
    drive_straight_gyro(2000, 700);
    line_square_up(650);
    drive_straight_gyro(2600, 700);
    set_servo_position(0, 700);
    drive_straight_gyro(660, 700);
    set_servo_position(0, 1600); //grabs poms
    msleep(2000);
    drive_straight_gyro(3600, 700);
    msleep(100);
    drive_straight_gyro(-4600, 700);
    line_square_up(700);
    drive_straight_gyro(-500, 700);
    turn_gyro(-40, 700);
    drive_straight_gyro(4000, 700);
    line_square_up(700);
    drive_straight_gyro(-700, 700);
    set_servo_position(0, 700);
    msleep(2000);
    disable_servos();
    return 0;
}
