#include "adjust.h"
#include "memory.h"

void update_setting(ADJUST_SETTING * setting) {
    setting->offset_x = *origin_x;
    setting->offset_y = *origin_y;
    setting->scale_x  = *scale_x;
    setting->scale_y  = *scale_y;
    setting->sin_rot  = sin(*rotate);
    setting->cos_rot  = cos(*rotate);
}

void adjust(double x, double y, Point * desc,
            ADJUST_SETTING * setting) {
    if (*scl_changed) {
        x *= setting->scale_x;
        y *= setting->scale_y;
    }
    if (*rot_changed) {
        x  = setting->cos_rot * x + y * setting->sin_rot;
        y  = setting->cos_rot * y - x * setting->sin_rot;
    }
    if (*org_changed) {
        x += setting->offset_x;
        y += setting->offset_y;
    }
    desc->x = x;
    desc->y = y;
}
