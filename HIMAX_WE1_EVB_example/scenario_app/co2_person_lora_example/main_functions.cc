/* Copyright 2019 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#include "hx_drv_tflm.h"

#include "scenario_app/co2_person_lora_example/main_functions.h"
#include "examples/person_detection/image_provider.h"
#include "examples/person_detection/model_settings.h"
#include "examples/person_detection/person_detect_model_data.h"
#include "scenario_app/co2_person_lora_example/settings.h"

#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/schema/schema_generated.h"


// Globals, used for compatibility with Arduino-style sketches.
namespace
{
  tflite::ErrorReporter *error_reporter = nullptr;
  const tflite::Model *model = nullptr;
  tflite::MicroInterpreter *interpreter = nullptr;
  TfLiteTensor *input = nullptr;
  bool lora_is_initialized = false;

  // In order to use optimized tensorflow lite kernels, a signed int8_t quantized
  // model is preferred over the legacy unsigned model format. This means that
  // throughout this project, input images must be converted from unisgned to
  // signed format. The easiest and quickest way to convert from unsigned to
  // signed 8-bit integers is to subtract 128 from the unsigned value to get a
  // signed value.

  // An area of memory to use for input, output, and intermediate arrays.
  constexpr int kTensorArenaSize = 136 * 1024;
#if (defined(__GNUC__) || defined(__GNUG__)) && !defined(__CCAC__)
  static uint8_t tensor_arena[kTensorArenaSize] __attribute__((section(".tensor_arena")));
#else
#pragma Bss(".tensor_arena")
  static uint8_t tensor_arena[kTensorArenaSize];
#pragma Bss()
#endif // if defined (_GNUC_) && !defined (_CCAC_)
} // namespace

bool read_AT_and_cmp(char *cmp_str)
{
  uint8_t recdata[10] = {};
  memset(recdata, '\0', sizeof(recdata));
  int count = 0;

  while (1)
  {
    if (hx_drv_swuart_single_read(recdata + count) == HX_DRV_LIB_PASS)
    {
      if (*(recdata + count) == 0xa)
      {
        count++;
        break;
      }
      count++;
    }
  }

  if (strcmp((char *)recdata, cmp_str) != 0)
  {
    return false;
  }

  return true;
}

bool send_AT_and_retry(char *senddata, int len, char *ack)
{
  int retry = 3;
  bool valid = false;
  for (int i = 0; i < retry; i++)
  {
    hx_drv_swuart_write((uint8_t *)senddata, len);
    if (read_AT_and_cmp(ack))
    {
      valid = true;
      break;
    }
  }
  return valid;
}

void lora_initial()
{
  if (!lora_is_initialized)
  {

    // init swuart
    hx_drv_swuart_initial(HX_DRV_PGPIO_0, HX_DRV_PGPIO_1, UART_BR_115200);

    // init loar
    if (!send_AT_and_retry((char *)"AT\x0d\x0a", 4, (char *)"+OK\x0d\x0a"))
    {
      hx_drv_uart_print("loar initial fail.");
      return;
    }
    if (!send_AT_and_retry((char *)"AT+ADDRESS=1\x0d\x0a", 14, (char *)"+OK\x0d\x0a"))
    {
      hx_drv_uart_print("loar initial fail.");
      return;
    }
    if (!send_AT_and_retry((char *)"AT+CRFOP=7\x0d\x0a", 12, (char *)"+OK\x0d\x0a"))
    {
      hx_drv_uart_print("loar initial fail.");
      return;
    }
    lora_is_initialized = true;
  }
}

// The name of this function is important for Arduino compatibility.
void setup()
{
  hx_drv_share_switch(SHARE_MODE_I2CM);
  // init uart
  hx_drv_uart_initial(UART_BR_115200);

  // init ccs811
  if (hx_drv_qwiic_ccs811_initial(HX_DRV_QWIIC_CCS811_I2C_0X5B) != HX_DRV_LIB_PASS)
  {
    hx_drv_uart_print("hx_drv_ccs811_initial fail.");
    return;
  }
  // init bme280
  if (hx_drv_qwiic_bme280_initial(HX_DRV_QWIIC_BME280_I2C_0X77) != HX_DRV_LIB_PASS)
  {
    hx_drv_uart_print("hx_drv_bme280_initial fail ");
    return;
  }

  // init model

  // Set up logging. Google style is to avoid globals or statics because of
  // lifetime uncertainty, but since this has a trivial destructor it's okay.
  // NOLINTNEXTLINE(runtime-global-variables)
  static tflite::MicroErrorReporter micro_error_reporter;
  error_reporter = &micro_error_reporter;

  // Map the model into a usable data structure. This doesn't involve any
  // copying or parsing, it's a very lightweight operation.
  model = tflite::GetModel(g_person_detect_model_data);
  if (model->version() != TFLITE_SCHEMA_VERSION)
  {
    TF_LITE_REPORT_ERROR(error_reporter,
                         "Model provided is schema version %d not equal "
                         "to supported version %d.",
                         model->version(), TFLITE_SCHEMA_VERSION);
    return;
  }

  // Pull in only the operation implementations we need.
  // This relies on a complete list of all the ops needed by this graph.
  // An easier approach is to just use the AllOpsResolver, but this will
  // incur some penalty in code space for op implementations that are not
  // needed by this graph.
  //
  // tflite::AllOpsResolver resolver;
  // NOLINTNEXTLINE(runtime-global-variables)
  static tflite::MicroMutableOpResolver<5> micro_op_resolver;
  micro_op_resolver.AddAveragePool2D();
  micro_op_resolver.AddConv2D();
  micro_op_resolver.AddDepthwiseConv2D();
  micro_op_resolver.AddReshape();
  micro_op_resolver.AddSoftmax();

  // Build an interpreter to run the model with.
  // NOLINTNEXTLINE(runtime-global-variables)
  static tflite::MicroInterpreter static_interpreter(
      model, micro_op_resolver, tensor_arena, kTensorArenaSize, error_reporter);
  interpreter = &static_interpreter;

  // Allocate memory from the tensor_arena for the model's tensors.
  TfLiteStatus allocate_status = interpreter->AllocateTensors();
  if (allocate_status != kTfLiteOk)
  {
    TF_LITE_REPORT_ERROR(error_reporter, "AllocateTensors() failed");
    return;
  }

  // Get information about the memory area to use for the model's input.
  input = interpreter->input(0);
}

// The name of this function is important for Arduino compatibility.
void loop()
{
  float t = 0, h = 0;
  uint32_t p = 0;
  uint16_t co2 = 0, tvoc = 0;
  char senddata[37] = {};

  if (hx_drv_qwiic_ccs811_get_data(&co2, &tvoc) == HX_DRV_LIB_PASS)
  {
    hx_drv_uart_print("CO2: %u ppm tVOC: %u ppb\n", co2, tvoc);
    if (hx_drv_qwiic_bme280_get_data(&t, &p, &h) == HX_DRV_LIB_PASS)
      hx_drv_uart_print("p:%d Pa, t:%d Celsius, h:%d %%RH\n", (uint32_t)p, (uint32_t)t, (uint32_t)h);

    // check CO2 >= CO2_THR
    if (co2 >= CO2_THR)
    {
      // Use sensor to detect person
      // Get image from provider.
      if (kTfLiteOk != GetImage(error_reporter, kNumCols, kNumRows, kNumChannels,
                                input->data.int8))
      {
        TF_LITE_REPORT_ERROR(error_reporter, "Image capture failed.");
      }

      // Run the model on this input and make sure it succeeds.
      if (kTfLiteOk != interpreter->Invoke())
      {
        TF_LITE_REPORT_ERROR(error_reporter, "Invoke failed.");
      }

      TfLiteTensor *output = interpreter->output(0);

      // Process the inference results.
      int8_t person_score = output->data.uint8[kPersonIndex];

      // if has person use lora send the message
      if (person_score >= PERSON_THR)
      {
        hx_drv_uart_print("has person!\n");
        lora_initial();
        if (lora_is_initialized)
        {
          sprintf((char *)senddata, (char *)"AT+SEND=2,22,%01d|%04d|%03d|%03d|%07d\x0d\x0a", 1, co2, (uint32_t)t, (uint32_t)h, (uint32_t)p);
          hx_drv_swuart_write((uint8_t *)senddata, 37);
          hx_drv_uart_print("send: %s", senddata);
          memset(senddata, '\0', sizeof(senddata));
        }
      }
    }
  }
}
