#include "create3_sensor.h"
#include <kipr/wombat.h>
#include <sys/param.h>
int create3_cliff_thresholds[4];
void calibrate_cliffs()
{
    int max_vals[4] = {-100, -100, -100, -100};
    int min_vals[4] = {100000, 100000, 100000, 100000};
    display_clear();
    display_printf(0, 0, "Set the create behind a black line");
    display_printf(0, 1, "Press A to start");
    while (!a_button_clicked())
    {
        msleep(100);
    }
    // debounce a button
    while (a_button())
        msleep(100);
    create3_velocity_set_components(0.1, 0);
    for (int i = 0; i < 5000; i++)
    {
        for (int sensor = 0; sensor < 4; sensor++)
        {
            create3_wait();
            int val = create3_sensor_cliff(sensor);
            display_printf(0, sensor + 2, "Cliff %d: %d    ", sensor, val);
            max_vals[sensor] = MAX(val, max_vals[sensor]);
            min_vals[sensor] = MIN(val, min_vals[sensor]);
        }
        if (a_button())
        {
            goto end_data_collection;
        }
        msleep(100);
    }
end_data_collection:
    for (int sensor = 0; sensor < 4; sensor++)
    {
        create3_cliff_thresholds[sensor] =
            (max_vals[sensor] + min_vals[sensor]) / 2;
    }
}
void display_create3_sensor_values()
{
    // display_clear();
    for (int i = 0; i < 4; i++)
    {
        int val = create3_sensor_cliff(i);
        display_printf(0, i, "cliff %d: %d         ", i, val);
    }
}
void create3_ir_squareup(int final_distance)
{
    const double responsiveness = 0.1 / 1000;
    int center = -10000;
    while (abs(center - final_distance) > 20)
    {
        display_create3_sensor_values();
        int left = create3_sensor_ir(2);
        int center = create3_sensor_ir(3);
        int right = create3_sensor_ir(4);
        create3_velocity_set_components(responsiveness * (final_distance - center),
                                        responsiveness * (left - right));
        create3_wait();
        msleep(20);
    }
}
float cliff_to_speed(float cliff_value)
{
    float error = cliff_value - CREATE3_LINE_THRESH;
    return error * error * error * 0.00015;
}
void create3_linesquare(enum create3_cliff_sensor left_sensor,
                        enum create3_cliff_sensor right_sensor, float speed)
{
    create3_velocity_set_components(speed, 0.0);
    int left;
    int right;
    do
    {
        left = create3_sensor_cliff(left_sensor);
        right = create3_sensor_cliff(right_sensor);
    } while (left > CREATE3_LINE_THRESH && right > CREATE3_LINE_THRESH);
    do
    {
        display_create3_sensor_values();
        left = create3_sensor_cliff(left_sensor);
        right = create3_sensor_cliff(right_sensor);
        float speed = cliff_to_speed((left + right) / 2.0);
        float rot = (right - left) / 5.0;
        create3_velocity_set_components(speed, rot);
    } while (abs(left - CREATE3_LINE_THRESH) > 5 ||
             abs(right - CREATE3_LINE_THRESH) > 5);
}