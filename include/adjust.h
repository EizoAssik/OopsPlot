#ifndef __OopsPlot__adjust__
#define __OopsPlot__adjust__

#include <stdio.h>
#include "memory.h"
typedef struct {
    double offset_x;
    double offset_y;
    double sin_rot;
    double cos_rot;
    double scale_x;
    double scale_y;
} ADJUST_SETTING;

void update_setting(ADJUST_SETTING * setting);

void adjust(double x, double y, Point * desc,
            ADJUST_SETTING * setting);

#endif /* defined(__OopsPlot__adjust__) */
