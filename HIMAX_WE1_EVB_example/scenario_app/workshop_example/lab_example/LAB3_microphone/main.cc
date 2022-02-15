#include "hx_drv_tflm.h"
#include "stdio.h"
#include "string.h"
#define mic_sample_rate 16000
#define AUD_BLK_100MS_SZ (mic_sample_rate / 1000 * 100)	//100ms
#define aud_stamp_cnt 30	//Record time: 30 * 100ms

typedef struct {
	int16_t left;
	int16_t right;
} META_AUDIO_t;

hx_drv_mic_data_config_t slt_audio_config;

META_AUDIO_t audio_clip[AUD_BLK_100MS_SZ * aud_stamp_cnt];

int main(int argc, char* argv[])
{
	int32_t time_prev = 0, time_cur = 0;
	uint8_t key_data;

	hx_drv_uart_initial(UART_BR_115200);

	//	Capture Dual channel audio data from Microphone. Each sample for dual PDM are 32 bits, 
	//	includes Left and right channel 16bits little-endian signed data.
	//	During each millisecond, there will be 16 samples(64 bytes) of audio data storage to target address.
	//	This API often called when get changes by API "hx_drv_mic_timestamp_get"
 	//	This API will retrieve latest 100ms audio data from microphone if return HX_DRV_LIB_PASS. 
	//	Once the API is done, you can get data from target address
	//	and wait next time stamp changes (which means wait 100ms) to get it return HX_DRV_LIB_PASS 
	//	(It will return HX_DRV_LIB_ERROR during time stamp changes).
	
	if(hx_drv_mic_initial() != HX_DRV_LIB_PASS)
		hx_drv_uart_print("Microphone Initialize Fail\n");
	else
		hx_drv_uart_print("Microphone Initialize Success\n");

 	if(hx_drv_mic_on() != HX_DRV_LIB_PASS)
		hx_drv_uart_print("Microphone Enable Fail\n");
	else
		hx_drv_uart_print("Microphone Enable Success\n");
	
	if(hx_drv_mic_timestamp_get(&time_prev) == HX_DRV_LIB_PASS)
	{
		time_cur = time_prev;
	}
	else 
		hx_drv_uart_print("Microphone Timestamp Error\n");

	hx_drv_uart_print("Wait for user press key: [R] \n");
	while (1) 
	{
		hx_drv_uart_getchar(&key_data);
		if(key_data == 'R')
		{			
			hx_drv_uart_print("Start Record Audio\n");
			for(int record_cnt = 0; record_cnt < aud_stamp_cnt; record_cnt ++)
			{
				while(time_cur == time_prev) 
				{
					//	Wait for time_stamp change
					//	It changes every 100ms
					hx_drv_mic_timestamp_get(&time_cur);
				}
				time_prev = time_cur;

				if(hx_drv_mic_capture_dual(&slt_audio_config) == HX_DRV_LIB_PASS) 
					memcpy((void*)&audio_clip[record_cnt * AUD_BLK_100MS_SZ], (void*)slt_audio_config.data_address, slt_audio_config.data_size*sizeof(uint8_t));
			}

			hx_drv_uart_print("Microphone Get Data Success\n");
			
			hx_drv_uart_print("Start to send\n");

			for(int i = 0; i < (AUD_BLK_100MS_SZ * aud_stamp_cnt); i ++)
			{
				hx_drv_uart_print("%5d, %7d, %7d\n", i, audio_clip[i].left,  audio_clip[i].right);
			}
			hx_drv_uart_print("End of send\n");
		}

		time_prev = time_cur;
		key_data = '\0';
	}
}
