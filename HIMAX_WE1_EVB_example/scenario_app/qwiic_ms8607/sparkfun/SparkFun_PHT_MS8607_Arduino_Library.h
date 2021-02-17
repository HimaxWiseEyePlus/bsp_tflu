/*
  This is a library written for the MS8607. Originally written by TEConnectivity
  with an MIT license. Library updated and brought to fit Arduino Library standards
  by PaulZC, October 30th, 2019. Based extensively on the Sparkfun_MS5637_Arduino_Library
  by Nathan Seidle @ SparkFun Electronics, April 13th, 2018.

  The MS8607 is a combined pressure, humidity and temperature sensor. The pressure
  sensor will operate down to 10mbar which is equivalent to an altitude of approx.
  31,000m making it suitable for high altitude ballooning as well as many other
  weather station applications.

  This library handles the initialization of the MS8607 and is able to
  query the sensor for different readings.

  Development environment specifics:
  Arduino IDE 1.8.10

  MIT License

  Copyright (c) 2016 TE Connectivity

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.

*/

#ifndef MS8607_ARDUINO_LIBRARY_H
#define MS8607_ARDUINO_LIBRARY_H

#define HIMAX_MS8607

#ifdef HIMAX_MS8607
#include "hx_drv_tflm.h"
#include "hx_example_utils.h"
#else
#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "Wire.h"

// Platform specific configurations
// Define Serial for SparkFun SAMD based boards.
// You may need to comment these three lines if your SAMD board supports Serial (not SerialUSB)
#if defined(ARDUINO_ARCH_SAMD)
#define Serial SerialUSB
#endif
#endif
// HSENSOR device address
const uint8_t MS8607_HSENSOR_ADDR = 0x40; // 0b1000000

// HSENSOR device commands
#define HSENSOR_RESET_COMMAND 0xFE
#define HSENSOR_READ_HUMIDITY_W_HOLD_COMMAND 0xE5
#define HSENSOR_READ_HUMIDITY_WO_HOLD_COMMAND 0xF5
#define HSENSOR_READ_SERIAL_FIRST_8BYTES_COMMAND 0xFA0F
#define HSENSOR_READ_SERIAL_LAST_6BYTES_COMMAND 0xFCC9
#define HSENSOR_WRITE_USER_REG_COMMAND 0xE6
#define HSENSOR_READ_USER_REG_COMMAND 0xE7

// Processing constants
#define HSENSOR_TEMPERATURE_COEFFICIENT (float)(-0.15)
#define HSENSOR_CONSTANT_A (float)(8.1332)
#define HSENSOR_CONSTANT_B (float)(1762.39)
#define HSENSOR_CONSTANT_C (float)(235.66)

// Coefficients for temperature computation
#define TEMPERATURE_COEFF_MUL (175.72)
#define TEMPERATURE_COEFF_ADD (-46.85)

// Coefficients for relative humidity computation
#define HUMIDITY_COEFF_MUL (125)
#define HUMIDITY_COEFF_ADD (-6)

// Conversion timings
#define HSENSOR_CONVERSION_TIME_12b 16
#define HSENSOR_CONVERSION_TIME_10b 5
#define HSENSOR_CONVERSION_TIME_8b 3
#define HSENSOR_CONVERSION_TIME_11b 9

#define HSENSOR_RESET_TIME 15 // ms value

// HSENSOR User Register masks and bit position
#define HSENSOR_USER_REG_RESOLUTION_MASK 0x81
#define HSENSOR_USER_REG_END_OF_BATTERY_MASK 0x40
#define HSENSOR_USER_REG_ENABLE_ONCHIP_HEATER_MASK 0x4
#define HSENSOR_USER_REG_DISABLE_OTP_RELOAD_MASK 0x2
#define HSENSOR_USER_REG_RESERVED_MASK                                         \
  (~(HSENSOR_USER_REG_RESOLUTION_MASK | HSENSOR_USER_REG_END_OF_BATTERY_MASK | \
     HSENSOR_USER_REG_ENABLE_ONCHIP_HEATER_MASK |                              \
     HSENSOR_USER_REG_DISABLE_OTP_RELOAD_MASK))

// HTU User Register values
// Resolution
#define HSENSOR_USER_REG_RESOLUTION_12b 0x00
#define HSENSOR_USER_REG_RESOLUTION_11b 0x81
#define HSENSOR_USER_REG_RESOLUTION_10b 0x80
#define HSENSOR_USER_REG_RESOLUTION_8b 0x01

// End of battery status
#define HSENSOR_USER_REG_END_OF_BATTERY_VDD_ABOVE_2_25V 0x00
#define HSENSOR_USER_REG_END_OF_BATTERY_VDD_BELOW_2_25V 0x40
// Enable on chip heater
#define HSENSOR_USER_REG_ONCHIP_HEATER_ENABLE 0x04
#define HSENSOR_USER_REG_OTP_RELOAD_DISABLE 0x02

// PSENSOR device address
const uint8_t MS8607_PSENSOR_ADDR = 0x76; // 0b1110110

// PSENSOR device commands
#define PSENSOR_RESET_COMMAND 0x1E
#define PSENSOR_START_PRESSURE_ADC_CONVERSION 0x40
#define PSENSOR_START_TEMPERATURE_ADC_CONVERSION 0x50
#define PSENSOR_READ_ADC 0x00

#define PSENSOR_CONVERSION_OSR_MASK 0x0F

#define PSENSOR_CONVERSION_TIME_OSR_256 1
#define PSENSOR_CONVERSION_TIME_OSR_512 2
#define PSENSOR_CONVERSION_TIME_OSR_1024 3
#define PSENSOR_CONVERSION_TIME_OSR_2048 5
#define PSENSOR_CONVERSION_TIME_OSR_4096 9
#define PSENSOR_CONVERSION_TIME_OSR_8192 18

// PSENSOR commands
#define PROM_ADDRESS_READ_ADDRESS_0 0xA0
#define PROM_ADDRESS_READ_ADDRESS_1 0xA2
#define PROM_ADDRESS_READ_ADDRESS_2 0xA4
#define PROM_ADDRESS_READ_ADDRESS_3 0xA6
#define PROM_ADDRESS_READ_ADDRESS_4 0xA8
#define PROM_ADDRESS_READ_ADDRESS_5 0xAA
#define PROM_ADDRESS_READ_ADDRESS_6 0xAC
#define PROM_ADDRESS_READ_ADDRESS_7 0xAE

// Coefficients indexes for temperature and pressure computation
#define CRC_INDEX 0
#define PRESSURE_SENSITIVITY_INDEX 1
#define PRESSURE_OFFSET_INDEX 2
#define TEMP_COEFF_OF_PRESSURE_SENSITIVITY_INDEX 3
#define TEMP_COEFF_OF_PRESSURE_OFFSET_INDEX 4
#define REFERENCE_TEMPERATURE_INDEX 5
#define TEMP_COEFF_OF_TEMPERATURE_INDEX 6
#define COEFFICIENT_NUMBERS 7

#define MAX_CONVERSION_TIME HSENSOR_CONVERSION_TIME_12b

enum MS8607_humidity_i2c_master_mode
{
       MS8607_i2c_hold,
       MS8607_i2c_no_hold
};

enum MS8607_status
{
       MS8607_status_ok,
       MS8607_status_no_i2c_acknowledge,
       MS8607_status_i2c_transfer_error,
       MS8607_status_crc_error,
       MS8607_status_heater_on_error
};

enum MS8607_humidity_resolution
{
       MS8607_humidity_resolution_12b = 0,
       MS8607_humidity_resolution_8b,
       MS8607_humidity_resolution_10b,
       MS8607_humidity_resolution_11b
};

enum MS8607_battery_status
{
       MS8607_battery_ok,
       MS8607_battery_low
};

enum MS8607_heater_status
{
       MS8607_heater_off,
       MS8607_heater_on
};

enum MS8607_pressure_resolution
{
       MS8607_pressure_resolution_osr_256 = 0,
       MS8607_pressure_resolution_osr_512,
       MS8607_pressure_resolution_osr_1024,
       MS8607_pressure_resolution_osr_2048,
       MS8607_pressure_resolution_osr_4096,
       MS8607_pressure_resolution_osr_8192
};

enum i2c_status_code
{
       i2c_status_ok = 0x00,
       i2c_status_err_overflow = 0x01,
       i2c_status_err_timeout = 0x02,
};

class MS8607
{

public:
       MS8607();

       /*
   \brief Perform initial configuration. Has to be called once.
  */
       #ifdef HIMAX_MS8607
       bool begin();
       #else
       bool begin(TwoWire &wirePort = Wire);
       #endif

       /*
  \brief Check whether MS8607 device is connected

  \return bool : status of MS8607
         - true : Device is present
         - false : Device is not acknowledging I2C address
  */
       bool isConnected(void);

       /*
   \brief Reset the MS8607 device

   \return MS8607_status : status of MS8607
          - MS8607_status_ok : I2C transfer completed successfully
          - MS8607_status_i2c_transfer_error : Problem with i2c transfer
          - MS8607_status_no_i2c_acknowledge : I2C did not acknowledge
  */
       enum MS8607_status reset(void);

       /*
   \brief Enable heater

   \return MS8607_status : status of MS8607
          - MS8607_status_ok : I2C transfer completed successfully
          - MS8607_status_i2c_transfer_error : Problem with i2c transfer
          - MS8607_status_no_i2c_acknowledge : I2C did not acknowledge
  */
       enum MS8607_status enable_heater(void);

       /*
   \brief Disable heater

   \return MS8607_status : status of MS8607
          - MS8607_status_ok : I2C transfer completed successfully
          - MS8607_status_i2c_transfer_error : Problem with i2c transfer
          - MS8607_status_no_i2c_acknowledge : I2C did not acknowledge
  */
       enum MS8607_status disable_heater(void);

       /*
   \brief Get heater status

   \param[in] MS8607_heater_status* : Return heater status (above or below 2.5V)
                       - MS8607_heater_off,
                        - MS8607_heater_on

   \return MS8607_status : status of MS8607
          - MS8607_status_ok : I2C transfer completed successfully
          - MS8607_status_i2c_transfer_error : Problem with i2c transfer
          - MS8607_status_no_i2c_acknowledge : I2C did not acknowledge
  */
       enum MS8607_status get_heater_status(enum MS8607_heater_status *heater);

       /*
   \brief Provide battery status

   \param[out] MS8607_battery_status* : Battery status
                         - MS8607_battery_ok,
                         - MS8607_battery_low

   \return MS8607_status : status of MS8607
          - MS8607_status_ok : I2C transfer completed successfully
          - MS8607_status_i2c_transfer_error : Problem with i2c transfer
          - MS8607_status_no_i2c_acknowledge : I2C did not acknowledge
  */
       enum MS8607_status get_battery_status(enum MS8607_battery_status *bat);

       /*
   \brief Reads the temperature, pressure and relative humidity value.

   \param[out] float* : degC temperature value
   \param[out] float* : mbar pressure value
   \param[out] float* : %RH Relative Humidity value

   \return MS8607_status : status of MS8607
          - MS8607_status_ok : I2C transfer completed successfully
          - MS8607_status_i2c_transfer_error : Problem with i2c transfer
          - MS8607_status_no_i2c_acknowledge : I2C did not acknowledge
          - MS8607_status_crc_error : CRC check error
  */
       enum MS8607_status read_temperature_pressure_humidity(float *t, float *p,
                                                             float *h);

       /******************** Functions from humidity sensor ********************/

       /*
   \brief Set humidity ADC resolution.

   \param[in] MS8607_humidity_resolution : Resolution requested

   \return MS8607_status : status of MS8607
          - MS8607_status_ok : I2C transfer completed successfully
          - MS8607_status_i2c_transfer_error : Problem with i2c transfer
          - MS8607_status_no_i2c_acknowledge : I2C did not acknowledge
          - MS8607_status_crc_error : CRC check error
  */
       enum MS8607_status
       set_humidity_resolution(enum MS8607_humidity_resolution res);

       /*
   \brief Set Humidity sensor ADC resolution.

   \param[in] MS8607_i2c_master_mode : I2C mode

   \return MS8607_status : status of MS8607
          - MS8607_status_ok
  */
       void set_humidity_i2c_master_mode(enum MS8607_humidity_i2c_master_mode mode);

       /*
   \brief Returns result of compensated humidity
          Note : This function shall only be used when the heater is OFF. It
          will return an error otherwise.

   \param[in] float - Actual temperature measured (degC)
   \param[in] float - Actual relative humidity measured (%RH)
   \param[out] float *- Compensated humidity (%RH).

   \return MS8607_status : status of MS8607
          - MS8607_status_ok : I2C transfer completed successfully
          - MS8607_status_heater_on_error : Cannot compute compensated humidity
            because heater is on
  */
       enum MS8607_status get_compensated_humidity(float temperature,
                                                   float relative_humidity,
                                                   float *compensated_humidity);

       /*
   \brief Returns the computed dew point
           Note : This function shall only be used when the heater is OFF. It
           will return an error otherwise.

   \param[in] float - Actual temperature measured (degC)
   \param[in] float - Actual relative humidity measured (%RH)
   \param[out] float *- Dew point temperature (DegC).

   \return MS8607_status : status of MS8607
          - MS8607_status_ok : I2C transfer completed successfully
          - MS8607_status_heater_on_error : Cannot compute compensated humidity
            because heater is on
  */
       enum MS8607_status get_dew_point(float temperature, float relative_humidity,
                                        float *dew_point);

       /******************** Functions from Pressure sensor ********************/
       /*
   \brief Set pressure ADC resolution.

   \param[in] MS8607_pressure_resolution : Resolution requested

  */
       void set_pressure_resolution(enum MS8607_pressure_resolution res);

       float getPressure();    //Returns the latest pressure measurement
       float getTemperature(); //Returns the latest temperature measurement
       float getHumidity();    //Returns the latest humidity measurement
       double adjustToSeaLevel(double absolutePressure, double actualAltitude);
       double altitudeChange(double currentPressure, double baselinePressure);


   // Storage for the 'global' parameters
   uint16_t eeprom_coeff[COEFFICIENT_NUMBERS + 1]; //Pressure sensor eeprom coefficients
   enum MS8607_pressure_resolution psensor_resolution_osr;


private:
       /******************** Functions from humidity sensor ********************/

       /*
   \brief Check whether humidity sensor is connected

   \return bool : status of humidity sensor
          - true : Device is present
          - false : Device is not acknowledging I2C address
  */
       bool hsensor_is_connected(void);

       /*
   \brief Reset the humidity sensor part

   \return MS8607_status : status of MS8607
          - MS8607_status_ok : I2C transfer completed successfully
          - MS8607_status_i2c_transfer_error : Problem with i2c transfer
          - MS8607_status_no_i2c_acknowledge : I2C did not acknowledge
  */
       enum MS8607_status hsensor_reset(void);

       /*
   \brief Check CRC

   \param[in] uint16_t : variable on which to check CRC
   \param[in] uint8_t : CRC value

   \return MS8607_status : status of MS8607
          - MS8607_status_ok : CRC check is OK
          - MS8607_status_crc_error : CRC check error
  */
       enum MS8607_status hsensor_crc_check(uint16_t value, uint8_t crc);

       /*
   \brief Reads the MS8607 humidity user register.

   \param[out] uint8_t* : Storage of user register value

   \return MS8607_status : status of MS8607
          - MS8607_status_ok : I2C transfer completed successfully
          - MS8607_status_i2c_transfer_error : Problem with i2c transfer
          - MS8607_status_no_i2c_acknowledge : I2C did not acknowledge
  */
       enum MS8607_status hsensor_read_user_register(uint8_t *value);

       /*
   \brief Writes the MS8607 humidity user register with value
           Will read and keep the unreserved bits of the register

   \param[in] uint8_t : Register value to be set.

   \return MS8607_status : status of MS8607
          - MS8607_status_ok : I2C transfer completed successfully
          - MS8607_status_i2c_transfer_error : Problem with i2c transfer
          - MS8607_status_no_i2c_acknowledge : I2C did not acknowledge
  */
       enum MS8607_status hsensor_write_user_register(uint8_t value);

       /*
   \brief Set Humidity sensor ADC resolution.

   \param[in] MS8607_i2c_master_mode : I2C mode

   \return MS8607_status : status of MS8607
          - MS8607_status_ok
  */
       enum MS8607_humidity_i2c_master_mode hsensor_i2c_master_mode;

       /*
   \brief Reads the relative humidity ADC value

   \param[out] uint16_t* : Relative humidity ADC value.

   \return MS8607_status : status of MS8607
          - MS8607_status_ok : I2C transfer completed successfully
          - MS8607_status_i2c_transfer_error : Problem with i2c transfer
          - MS8607_status_no_i2c_acknowledge : I2C did not acknowledge
          - MS8607_status_crc_error : CRC check error
  */
       enum MS8607_status hsensor_humidity_conversion_and_read_adc(uint16_t *adc);

       /*
   \brief Reads the relative humidity value.

   \param[out] float* : %RH Relative Humidity value

   \return MS8607_status : status of MS8607
          - MS8607_status_ok : I2C transfer completed successfully
          - MS8607_status_i2c_transfer_error : Problem with i2c transfer
          - MS8607_status_no_i2c_acknowledge : I2C did not acknowledge
          - MS8607_status_crc_error : CRC check error
  */
       enum MS8607_status hsensor_read_relative_humidity(float *humidity);

       /******************** Functions from Pressure sensor ********************/

       /*
   \brief Check whether MS8607 pressure sensor is connected

   \return bool : status of MS8607
          - true : Device is present
          - false : Device is not acknowledging I2C address
  */
       bool psensor_is_connected(void);

       /*
   \brief Reset the Pressure sensor part

   \return MS8607_status : status of MS8607
          - MS8607_status_ok : I2C transfer completed successfully
          - MS8607_status_i2c_transfer_error : Problem with i2c transfer
          - MS8607_status_no_i2c_acknowledge : I2C did not acknowledge
  */
       enum MS8607_status psensor_reset(void);

       /*
   \brief Reads the psensor EEPROM coefficient stored at address provided.

   \param[in] uint8_t : Address of coefficient in EEPROM
   \param[out] uint16_t* : Value read in EEPROM

   \return MS8607_status : status of MS8607
          - MS8607_status_ok : I2C transfer completed successfully
          - MS8607_status_i2c_transfer_error : Problem with i2c transfer
          - MS8607_status_no_i2c_acknowledge : I2C did not acknowledge
          - MS8607_status_crc_error : CRC check error on the coefficients
  */
       enum MS8607_status psensor_read_eeprom_coeff(uint8_t command,
                                                    uint16_t *coeff);

       /*
   \brief Reads the MS8607 EEPROM coefficients to store them for computation.

   \return MS8607_status : status of MS8607
          - MS8607_status_ok : I2C transfer completed successfully
          - MS8607_status_i2c_transfer_error : Problem with i2c transfer
          - MS8607_status_no_i2c_acknowledge : I2C did not acknowledge
          - MS8607_status_crc_error : CRC check error on the coefficients
  */
       enum MS8607_status psensor_read_eeprom(void);

       /*
   \brief CRC check

   \param[in] uint16_t *: List of EEPROM coefficients
   \param[in] uint8_t : crc to compare

   \return bool : TRUE if CRC is OK, FALSE if KO
  */
       bool psensor_crc_check(uint16_t *n_prom, uint8_t crc);

       /*
   \brief Compute temperature and pressure

   \param[out] float* : Celsius Degree temperature value
   \param[out] float* : mbar pressure value

   \return MS8607_status : status of MS8607
          - MS8607_status_ok : I2C transfer completed successfully
          - MS8607_status_i2c_transfer_error : Problem with i2c transfer
          - MS8607_status_no_i2c_acknowledge : I2C did not acknowledge
          - MS8607_status_crc_error : CRC check error on the coefficients
  */
       enum MS8607_status psensor_read_pressure_and_temperature(float *temperature,
                                                                float *pressure);

       /*
   \brief Triggers conversion and read ADC value

   \param[in] uint8_t : Command used for conversion (will determine
    Temperature vs Pressure and osr)
   \param[out] uint32_t* : ADC value.

   \return MS8607_status : status of MS8607
          - MS8607_status_ok : I2C transfer completed successfully
          - MS8607_status_i2c_transfer_error : Problem with i2c transfer
          - MS8607_status_no_i2c_acknowledge : I2C did not acknowledge
  */
       enum MS8607_status psensor_conversion_and_read_adc(uint8_t cmd,
                                                          uint32_t *adc);

       uint32_t hsensor_conversion_time;
       bool hsensor_heater_on;
       uint32_t psensor_conversion_time[6];
       
       #ifdef HIMAX_MS8607
       #else
       TwoWire *_i2cPort; //The generic connection to user's chosen I2C hardware
       #endif
       float globalPressure;
       bool pressureHasBeenRead = true;
       float globalTemperature;
       bool temperatureHasBeenRead = true;
       float globalHumidity;
       bool humidityHasBeenRead = true;
};
#endif
