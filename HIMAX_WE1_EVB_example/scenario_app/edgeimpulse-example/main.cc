#include "hx_drv_tflm.h"
#include "tflite-model/trained_model_compiled.h"

namespace
{
    hx_drv_sensor_image_config_t g_pimg_config;
    hx_drv_gpio_config_t gpio_config;
    int input_width = 96;
    int input_height = 96;
    int input_channels = 1;
}

TfLiteStatus GetImage(int image_width, int image_height, int channels, int8_t *image_data)
{
    static bool is_initialized = false;

    if (!is_initialized)
    {
        if (hx_drv_sensor_initial(&g_pimg_config) != HX_DRV_LIB_PASS)
        {
            return kTfLiteError;
        }

        if (hx_drv_spim_init() != HX_DRV_LIB_PASS)
        {
            return kTfLiteError;
        }

        is_initialized = true;
    }

    //capture image by sensor
    hx_drv_sensor_capture(&g_pimg_config);

    //send jpeg image data out through SPI
    hx_drv_spim_send(g_pimg_config.jpeg_address, g_pimg_config.jpeg_size,
                     SPI_TYPE_JPG);

    hx_drv_image_rescale((uint8_t *)g_pimg_config.raw_address,
                         g_pimg_config.img_width, g_pimg_config.img_height,
                         image_data, image_width, image_height);

    return kTfLiteOk;
}

void RespondToDetection(int8_t *score)
{
    // get the index with the highest score
    int maxindex = 0;
    int maxvalue = -128;
    for (int i = 0; i < 4; i++)
    {
        if (score[i] > maxvalue)
        {
            maxvalue = score[i];
            maxindex = i;
        }
    }

    // init gpio pin to 0
    gpio_config.gpio_pin = HX_DRV_PGPIO_0;
    gpio_config.gpio_data = 0;
    hx_drv_gpio_set(&gpio_config);
    gpio_config.gpio_pin = HX_DRV_PGPIO_1;
    gpio_config.gpio_data = 0;
    hx_drv_gpio_set(&gpio_config);
    gpio_config.gpio_pin = HX_DRV_PGPIO_2;
    gpio_config.gpio_data = 0;
    hx_drv_gpio_set(&gpio_config);

    switch (maxindex)
    {
    case 0:
        hx_drv_led_off(HX_DRV_LED_GREEN);
        hx_drv_led_off(HX_DRV_LED_RED);
        break;
    case 1:
        // if detect gesture 1, turn on LED GREEN and output 1 to PGPIO_0
        hx_drv_led_on(HX_DRV_LED_GREEN);
        hx_drv_led_off(HX_DRV_LED_RED);
        gpio_config.gpio_pin = HX_DRV_PGPIO_0;
        gpio_config.gpio_data = 1;
        hx_drv_gpio_set(&gpio_config);
        break;
    case 2:
        // if detect gesture 2, turn on LED RED and output 1 to PGPIO_1
        hx_drv_led_off(HX_DRV_LED_GREEN);
        hx_drv_led_on(HX_DRV_LED_RED);
        gpio_config.gpio_pin = HX_DRV_PGPIO_1;
        gpio_config.gpio_data = 1;
        hx_drv_gpio_set(&gpio_config);
        break;
    case 3:
        // if detect gesture 3, turn on LED GREEN & RED and output 1 to PGPIO_2
        hx_drv_led_on(HX_DRV_LED_GREEN);
        hx_drv_led_on(HX_DRV_LED_RED);
        gpio_config.gpio_pin = HX_DRV_PGPIO_2;
        gpio_config.gpio_data = 1;
        hx_drv_gpio_set(&gpio_config);
        break;
    }
    // show the scores to UART
    hx_drv_uart_print("[0] %d, [1] %d [2] %d [3] %d\n", score[0], score[1], score[2], score[3]);
}

int main(void)
{
    // initial uart and gpio
    hx_drv_uart_initial(UART_BR_115200);
    gpio_config.gpio_pin = HX_DRV_PGPIO_0;
    gpio_config.gpio_direction = HX_DRV_GPIO_OUTPUT;
    hx_drv_gpio_initial(&gpio_config);
    gpio_config.gpio_pin = HX_DRV_PGPIO_1;
    hx_drv_gpio_initial(&gpio_config);
    gpio_config.gpio_pin = HX_DRV_PGPIO_2;
    hx_drv_gpio_initial(&gpio_config);

    // init model
    TfLiteStatus init_status = trained_model_init(NULL);
    if (init_status != kTfLiteOk)
    {
        hx_drv_uart_print("init fail\n");
        return 0;
    }

    TfLiteTensor *input = trained_model_input(0);

    // loop step
    while (true)
    {
        if (kTfLiteOk != GetImage(input_width, input_height, input_channels, input->data.int8))
        {
            hx_drv_uart_print("Image capture failed.");
            continue;
        }
        trained_model_invoke();

        TfLiteTensor *output = trained_model_output(0);

        RespondToDetection((int8_t *)output->data.uint8);
    }

    return 0;
}