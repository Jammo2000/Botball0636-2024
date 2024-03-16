enum create3_cliff_sensor
{
    CLIFF_RIGHT = 3,
    CLIFF_CENTER_RIGHT = 2,
    CLIFF_CENTER_LEFT = 1,
    CLIFF_LEFT = 0
};
enum create3_bump_sensor
{
    BUMP_RIGHT = 3,
    BUMP_CENTER_RIGHT = 2,
    BUMP_CENTER_LEFT = 1,
    BUMP_LEFT = 0
};
#define CREATE3_LINE_THRESH 2180
extern int create3_cliff_thresholds[4];
void calibrate_cliffs();
void display_create3_sensor_values();
void create3_ir_squareup(int final_distance);
float cliff_to_speed(float cliff_value);
void create3_linesquare(enum create3_cliff_sensor left_sensor, enum create3_cliff_sensor rights_sensor, float speed);