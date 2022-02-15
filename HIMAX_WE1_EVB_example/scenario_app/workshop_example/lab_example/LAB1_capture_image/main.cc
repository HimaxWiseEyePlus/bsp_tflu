#include "hx_drv_tflm.h"
#include <stdio.h>

hx_drv_sensor_image_config_t pimg_config;

HX_DRV_ERROR_E GetImage(void)
{
    //capture image by sensor
    hx_drv_sensor_capture(&pimg_config);

    hx_drv_uart_print("\nimg_height: %d\n",pimg_config.img_height);
    hx_drv_uart_print("img_width:  %d\n",pimg_config.img_width);

    //Camera image send by SPI
    if(hx_drv_share_switch(SHARE_MODE_SPIM) != HX_DRV_LIB_PASS)
    {
        hx_drv_uart_print("Switch to SPI fail.");
        return HX_DRV_LIB_ERROR;
    }
    if(hx_drv_spim_send(pimg_config.jpeg_address , pimg_config.jpeg_size , SPI_TYPE_JPG) != HX_DRV_LIB_PASS)
    {
        hx_drv_uart_print("Image capture failed.");
        return HX_DRV_LIB_ERROR;
    }
    hx_drv_uart_print("Image capture successed.\n");
    return HX_DRV_LIB_PASS;
}




int main(int argc, char *argv[])
{
    // initial uart
    hx_drv_uart_initial(UART_BR_115200);
    
    if (hx_drv_sensor_initial(&pimg_config) != HX_DRV_LIB_PASS)
    {
        hx_drv_uart_print("Sensor initial fail.");
        return 0;
    }

    if (hx_drv_spim_init() != HX_DRV_LIB_PASS)
    {
        hx_drv_uart_print("Spi initial fail.");
        return 0;
    }

    while (1)
    {
        if (GetImage() == HX_DRV_LIB_ERROR)
        {
           
            continue;
        }
    }

    return 0;
}