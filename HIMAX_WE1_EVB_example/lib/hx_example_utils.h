#ifndef HIMAX_EMAMPLE_UTILS
#define HIMAX_EMAMPLE_UTILS

#include "hx_drv_tflm.h"

inline void hx_util_delay_ms(uint32_t tick)
{
    uint32_t tick_end = 0;
    hx_drv_tick_start();
    while ((double)tick_end/400000.0 < tick)
    {
        hx_drv_tick_get(&tick_end);
    }
    return;
}

#endif