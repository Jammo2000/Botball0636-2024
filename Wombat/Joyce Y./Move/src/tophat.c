#include "tophat.h"
#include <kipr/wombat.h>
#include <stdlib.h>
#include "constants.h"



const double line_follow_aggression = 0.0000004;
const char* tophat_file_path = "/home/kipr/Documents/KISS/Joyce Y./Move/data/tophat_settings.dat";
/*
API
 calibrate_tophats() 
 	- follow the prompts on the wombat display to calibrate the line following.
 line_square_up(int maxSpeed) 
 	- drives forward until it hits a line, and then rotates until it is perpendicular to the line


*/
//define ports
struct ts {
    int right_white;
    int right_black;
    int right_thresh;
    int left_white;
    int left_black;
    int left_thresh;
};

struct ts tophat_settings = {
    175,
    3000,
    1587,
    175,
    3000,
    1587
};
int save_tophat_settings() {
    FILE *file = fopen(tophat_file_path, "w");
    if (file == NULL)
        return -1;
    int file_res =
        fprintf(file, "%d\n%d\n%d\n%d\n%d\n%d", tophat_settings.right_white,
                tophat_settings.right_black, tophat_settings.right_thresh,
                tophat_settings.left_white, tophat_settings.left_black,
                tophat_settings.left_thresh);
    fclose(file);
    if (file_res < 0)
        return -1;
    return 0;
}
int load_tophat_settings(){
    FILE *file = fopen(tophat_file_path, "r");
    if (file == NULL)
        return -1;
    int file_res =
        fscanf(file, "%d\n%d\n%d\n%d\n%d\n%d", &tophat_settings.right_white,
               &tophat_settings.right_black, &tophat_settings.right_thresh,
               &tophat_settings.left_white, &tophat_settings.left_black,
               &tophat_settings.left_thresh);
    fclose(file);
    if (file_res != 6)
        return -1;
    return 0;
}
// Edits the global "tophat_settings"
void calibrate_tophats(){
    display_clear();
    display_printf(0, 0, "Press A to calibrate tophats");
    display_printf(0, 1, "Press B to load tophat settings from save");
    int aorb = 0;
    while (aorb==0) {
        if(a_button_clicked()) aorb = 1;
        if(b_button_clicked()) aorb = 2;
        msleep(10);
    }
    if(aorb == 2){
        int res = load_tophat_settings();
        // load tophat settings
        if(res == 0){
            // success
            return;
        }else{
            display_clear();
            display_printf(0, 0, "Tophat loading failed: code %d", res);
            msleep(3000);
        }
    }
    calibrate:
    display_clear();
    //get white values
    display_printf(0,0,"Place both tophats over black");
    display_printf(0,1,"Press A when finished");
    while(!a_button_clicked()){
        display_printf(0,2,"right value: %d      ", analog(right_tophat));
        display_printf(0,3,"left value: %d      ", analog(left_tophat));
        msleep(50);
    }
    tophat_settings.right_black = analog(right_tophat);
    tophat_settings.left_black = analog(left_tophat);
    //get black values
    display_printf(0,4,"Place both tophats over white  ");
    display_printf(0,5,"Press A when finished ");
    while(!a_button_clicked()){
        display_printf(0,6,"right value: %d      ", analog(right_tophat));
        display_printf(0,7,"left value: %d      ", analog(left_tophat));
        msleep(50);
    }
    tophat_settings.right_white = analog(right_tophat);
    tophat_settings.left_white = analog(left_tophat);
    if(abs(tophat_settings.right_white-tophat_settings.right_black)<500){
        display_clear();
        display_printf(0, 0, "Bad calibration. Check the right tophat sensor");
        display_printf(0, 1, "It should be plugged into port %d", right_tophat);
        msleep(4000);
        goto calibrate;
    }
    if(abs(tophat_settings.left_white-tophat_settings.left_black)<500){
        display_clear();
        display_printf(0, 0, "Bad calibration. Check the left tophat sensor");
        display_printf(0, 1, "It should be plugged into port %d", left_tophat);
        msleep(4000);
        goto calibrate;
    }
    tophat_settings.right_thresh = (tophat_settings.right_black+tophat_settings.right_white)/2;
    tophat_settings.left_thresh = (tophat_settings.left_black+tophat_settings.left_white)/2;
    display_clear();
    display_printf(0, 0, "Press A to save these settings");
    display_printf(0, 1, "Press B to not save them");
    aorb = 0;
    while (aorb==0) {
        if(a_button_clicked()) aorb = 1;
        if(b_button_clicked()) aorb = 2;
        msleep(10);
    }
    if(aorb==1) {
        int res = save_tophat_settings();
        display_clear();
        if(res == 0){
            display_printf(0, 0, "Saved");
        }else{
            display_printf(0, 0, "Failed to save");
        }
    }
    msleep(2000);
    display_clear();
}

void drive_direct(int left, int right){
    mav(right_wheel, right);
    mav(left_wheel, left);
}
// Speed can't be too high or else it will fail

void line_follow_left(int distance, int speed, enum line_follow_side side){
    cmpc(right_wheel);
    cmpc(left_wheel);
    double slope = (line_follow_aggression*speed)/(tophat_settings.left_white-tophat_settings.left_black);
    while((gmpc(right_wheel)+gmpc(left_wheel))/2 < distance){
        int tophat_val = analog(left_tophat);
        int diff = tophat_val - tophat_settings.left_thresh;
        double diff3 = (double)diff*diff*diff;
        double left_speed = side * slope * diff3 + speed;
        double right_speed = -side * slope * diff3 + speed;
        drive_direct(
            left_speed,
            right_speed
        );
        msleep(20);
    }
    off(left_wheel);
    off(right_wheel);

}
void line_follow_right(int distance, int speed, enum line_follow_side side){
    cmpc(right_wheel);
    cmpc(left_wheel);
    double slope = (line_follow_aggression*speed)/(tophat_settings.right_white-tophat_settings.right_black);
    while((gmpc(right_wheel)+gmpc(left_wheel))/2 < distance){
        int tophat_val = analog(right_tophat);
        int diff = tophat_val - tophat_settings.right_thresh;
        double diff3 = (double)diff*diff*diff;
        double left_speed = -side * slope * diff3 + speed;
        double right_speed = side * slope * diff3 + speed;
        drive_direct(
            left_speed,
            right_speed
        );
        msleep(20);
    }
    off(left_wheel);
    off(right_wheel);

}
void line_follow_right_until_touch(int speed, enum line_follow_side side){
    cmpc(right_wheel);
    cmpc(left_wheel);
    double slope = (line_follow_aggression*speed)/(tophat_settings.right_white-tophat_settings.right_black);
    while(!digital(touch_sensor)){
        int tophat_val = analog(right_tophat);
        int diff = tophat_val - tophat_settings.right_thresh;
        double diff3 = (double)diff*diff*diff;
        double left_speed = -side * slope * diff3 + speed;
        double right_speed = side * slope * diff3 + speed;
        drive_direct(
            left_speed,
            right_speed
        );
        msleep(20);
    }
    off(left_wheel);
    off(right_wheel);

}
void line_square_up(int maxSpeed){
    //slopes for linear function between light and speed

    float right_slope = 1.2f*maxSpeed/(tophat_settings.right_white-tophat_settings.right_black);
    float left_slope = 1.2f*maxSpeed/(tophat_settings.left_white-tophat_settings.left_black);

    int right, left;
    float right_speed;
    float left_speed;

    while(analog(right_tophat)<tophat_settings.right_thresh&&analog(left_tophat)<tophat_settings.left_thresh){	
        drive_direct(maxSpeed, maxSpeed);
        msleep(10);
    }

    do{
        right = analog(right_tophat);
        left = analog(left_tophat);
        right_speed = right_slope * (right-tophat_settings.right_white) + maxSpeed;
        left_speed = left_slope*(left-tophat_settings.left_white) + maxSpeed;
        drive_direct(left_speed, right_speed);
        msleep(10);
    }while(
        right_speed>10||right_speed<-10
        ||left_speed>10||left_speed<-10
    );
    off(left_wheel);
    off(right_wheel);
}

#ifdef ET_H // These functions depend on the et.h stuff
int denoise_et(int samples, int spacing){
    double res = analog(et);
    int i;
    printf("[");
    for(i=0;i<samples-1;i++){
        msleep(spacing);
        double val = analog(et);
        printf("%.1f, ", val);

        res += val;
    }
    res  = res / samples;
    printf("%.1f]\n", res);
    return (int)res;
}
// Uses the ET sensor, and is calibrated by et.h
// Will follow until analog(et) is >= et_settings
void line_follow_left_et(int speed, enum line_follow_side side){
    cmpc(right_wheel);
    cmpc(left_wheel);
    double slope = (line_follow_aggression*speed)/(tophat_settings.left_white-tophat_settings.left_black);
    while(denoise_et(5, 10)<et_settings){
        int tophat_val = analog(left_tophat);
        int diff = tophat_val - tophat_settings.left_thresh;
        double diff3 = (double)diff*diff*diff;
        double left_speed = side * slope * diff3 + speed;
        double right_speed = -side * slope * diff3 + speed;
        drive_direct(
            left_speed,
            right_speed
        );
    }
    off(left_wheel);
    off(right_wheel);
}
#endif //ET_H