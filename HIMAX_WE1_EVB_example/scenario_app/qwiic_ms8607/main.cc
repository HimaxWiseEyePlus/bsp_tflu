#include "hx_drv_tflm.h"

int main(int argc, char *argv[])
{
    hx_drv_share_switch(SHARE_MODE_I2CM);
    hx_drv_uart_initial(UART_BR_115200);

    if (hx_drv_qwiic_ms8607_initial() != HX_DRV_LIB_PASS)
    {
        hx_drv_uart_print("hx_drv_ms8607_initial fail ");
        return 0;
    }

    float p = 0, t = 0, h = 0;
    while (true)
    {
        if (hx_drv_qwiic_ms8607_get_data(&t, &p, &h) == HX_DRV_LIB_PASS)
            hx_drv_uart_print("p:%d mbar, t:%d Celsius, h:%d %%RH\n", (uint32_t)p, (int32_t)t, (uint32_t)h);
    }

    return 0;
}