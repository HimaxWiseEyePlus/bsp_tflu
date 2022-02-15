#include "hx_drv_tflm.h"
#include<stdio.h>
#define scale_size 10
int main(int argc, char* argv[])
{
	hx_drv_uart_initial(UART_BR_115200);

	//It will initial accelerometer with sampling rate 119 Hz, bandwidth 50 Hz, scale selection 4g at continuous mode.
	//Accelerometer operates in FIFO mode. 
	//FIFO size is 32
	if (hx_drv_accelerometer_initial() != HX_DRV_LIB_PASS)
		hx_drv_uart_print("Accelerometer Initialize Fail\n");
	else
		hx_drv_uart_print("Accelerometer Initialize Success\n");

	while (1) 
	{
		int available_count = 0;
		float x, y, z;
		available_count = hx_drv_accelerometer_available_count();				
		hx_drv_uart_print("Accel get FIFO: %d\n", available_count);
		for (int i = 0; i < available_count; i++) 
		{
			hx_drv_accelerometer_receive(&x, &y, &z);
		}
		char x_sign = '+';
		int32_t x_int_buf = x * scale_size;
		uint32_t pos_x = 0;
		uint32_t flo_x = 0;
		if(x_int_buf < 0)
		{
			x_sign = '-';
			x_int_buf = x_int_buf * (-1);
		}
		pos_x = x_int_buf / scale_size;
		flo_x = x_int_buf % scale_size;
		char x_char[11];
    	sprintf(x_char, "x= %c%1d.%1d ", x_sign,pos_x,flo_x);
		hx_drv_uart_print(x_char);
		
		

		char y_sign = '+';
		int32_t y_int_buf = y * scale_size;
		uint32_t pos_y = 0;
		uint32_t flo_y = 0;
		if(y_int_buf < 0)
		{
			y_sign = '-';
			y_int_buf = y_int_buf * (-1);
		}
		pos_y = y_int_buf / scale_size;
		flo_y = y_int_buf % scale_size;
		char y_char[11];
    	sprintf(y_char, "y= %c%1d.%1d ", y_sign,pos_y,flo_y);
		hx_drv_uart_print(y_char);
		
		


		char z_sign = '+';
		int32_t z_int_buf = z * scale_size;
		uint32_t pos_z = 0;
		uint32_t flo_z = 0;
		if(z_int_buf < 0)
		{
			z_sign = '-';
			z_int_buf = z_int_buf * (-1);
		}
		pos_z = z_int_buf / scale_size;
		flo_z = z_int_buf % scale_size;
		char z_char[11];
    	sprintf(z_char, "z= %c%1d.%1d \n", z_sign,pos_z,flo_z);
		hx_drv_uart_print(z_char);
		
		//delay
		for(int i=0;i<1000;i++)
		{
			hx_drv_uart_print(" ");
		}
		hx_drv_uart_print("\n");
	}
}
