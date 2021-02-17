#include "SparkFun_PHT_MS8607_Arduino_Library.h"
#include "hx_drv_tflm.h"

MS8607 mySensor;

void setup()
{
    hx_drv_share_switch(SHARE_MODE_I2CM);
    hx_drv_uart_initial(UART_BR_115200);

    hx_drv_uart_print("MS8607\n");

    if (mySensor.begin() == false)
    {
        hx_drv_uart_print("MS8607 error. Please check wiring. Freezing...\n");
        while (1)
            ;
    }
}

void loop()
{
    float t, p, h;
    if (mySensor.read_temperature_pressure_humidity(&t, &p, &h) == MS8607_status_ok)
    {
        hx_drv_uart_print("p:%d mbar, t:%d Celsius, h:%d %%RH\n", (uint32_t)p, (int32_t)t, (uint32_t)h);
    }
}

int main(int argc, char *argv[])
{
    setup();
    for (int i = 0; i < 10; i++)
    {
        loop();
    }
    return 0;
}