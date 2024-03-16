#include <kipr/wombat.h>
#include "create3_sensor.h"
int main()
{
    int success = create3_is_connected();
    printf("Success: %d\n", success);
    if (!success)
        success = create3_connect();
    printf("Success: %d\n", success);
    if (!success)
        return 1;
    display_clear();
    create3_ir_squareup(1000);

    /*Create3Vector3 pos = create3_pose_get().position;
    create3_navigate_to_position_with_heading(0, 0, 0, 0.3, 2.5);
        printf("position: (%f, %f, %f)\n", pos.x, pos.y, pos.z);
    create3_drive_arc_degrees(0.5, 90, 5);
    create3_rotate_degrees(180-45, 300);
    create3_drive_straight(hypot(0.5, 0.5), 1);
    create3_rotate_degrees(45+90, 300);
    Create3Vector3 pos1 = create3_pose_get().position;
    printf("change in pos: (%f, %f, %f)\n", pos1.x-pos.x, pos1.y-pos.y, pos1.z-pos.z);*/
    return 0;
}
