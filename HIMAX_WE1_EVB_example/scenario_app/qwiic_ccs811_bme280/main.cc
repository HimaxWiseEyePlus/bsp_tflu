#include "hx_drv_tflm.h"

int main(int argc, char *argv[])
{
    hx_drv_share_switch(SHARE_MODE_I2CM);
    hx_drv_uart_initial(UART_BR_115200);

    if (hx_drv_qwiic_ccs811_initial(HX_DRV_QWIIC_CCS811_I2C_0X5B) != HX_DRV_LIB_PASS)
    {
        hx_drv_uart_print("hx_drv_ccs811_initial fail.");
        return 0;
    }
    if (hx_drv_qwiic_bme280_initial(HX_DRV_QWIIC_BME280_I2C_0X77) != HX_DRV_LIB_PASS)
    {
        hx_drv_uart_print("hx_drv_bme280_initial fail ");
        return 0;
    }

    float t = 0, h = 0;
    uint32_t p = 0;
    uint16_t co2 = 0, tvoc = 0;

    while (true)
    {

        if (hx_drv_qwiic_ccs811_get_data(&co2, &tvoc) == HX_DRV_LIB_PASS)
        {
            hx_drv_uart_print("CO2: %u ppm tVOC: %u ppb\n", co2, tvoc);

            if (hx_drv_qwiic_bme280_get_data(&t, &p, &h) == HX_DRV_LIB_PASS)
                hx_drv_uart_print("p:%d Pa, t:%d Celsius, h:%d %%RH\n", (uint32_t)p, (uint32_t)t, (uint32_t)h);
        }
    }

    return 0;
}