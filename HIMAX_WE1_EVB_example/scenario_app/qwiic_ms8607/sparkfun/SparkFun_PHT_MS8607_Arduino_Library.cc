#include "SparkFun_PHT_MS8607_Arduino_Library.h"
#include "cmath"

#ifdef HIMAX_MS8607
#else
#include <Wire.h>
#endif

MS8607::MS8607(void)
    : psensor_conversion_time{
          PSENSOR_CONVERSION_TIME_OSR_256, PSENSOR_CONVERSION_TIME_OSR_512,
          PSENSOR_CONVERSION_TIME_OSR_1024, PSENSOR_CONVERSION_TIME_OSR_2048,
          PSENSOR_CONVERSION_TIME_OSR_4096, PSENSOR_CONVERSION_TIME_OSR_8192}
{

  hsensor_conversion_time = HSENSOR_CONVERSION_TIME_12b;
  hsensor_i2c_master_mode = MS8607_i2c_no_hold;
  hsensor_heater_on = false;
}

/*
  \brief Perform initial configuration. Has to be called once.
*/
#ifdef HIMAX_MS8607
bool MS8607::begin()
{
  //Check connection
  if (isConnected() == false)
    return (false);

  //Get EEPROM coefficients
  enum MS8607_status status = psensor_read_eeprom();
  if (status != MS8607_status_ok)
    return (false);

  //Set resolution to the highest level (17 ms per reading)
  psensor_resolution_osr = MS8607_pressure_resolution_osr_8192;

  return (true);
}
#else
bool MS8607::begin(TwoWire &wirePort)
{
  _i2cPort = &wirePort; //Grab which port the user wants us to use

  //Check connection
  if (isConnected() == false)
    return (false);

  //Get EEPROM coefficients
  enum MS8607_status status = psensor_read_eeprom();
  if (status != MS8607_status_ok)
    return (false);

  //Set resolution to the highest level (17 ms per reading)
  psensor_resolution_osr = MS8607_pressure_resolution_osr_8192;

  return (true);
}
#endif

/*
  \brief Check whether MS8607 device is connected

  \return bool : status of MS8607
       - true : Device is present
       - false : Device is not acknowledging I2C address
*/
bool MS8607::isConnected(void)
{
  return (hsensor_is_connected() && psensor_is_connected());
}

/*
  \brief Reset the MS8607 device

  \return MS8607_status : status of MS8607
       - MS8607_status_ok : I2C transfer completed successfully
       - MS8607_status_i2c_transfer_error : Problem with i2c transfer
       - MS8607_status_no_i2c_acknowledge : I2C did not acknowledge
*/
enum MS8607_status MS8607::reset(void)
{
  enum MS8607_status status;

  status = hsensor_reset();
  if (status != MS8607_status_ok)
    return status;
  status = psensor_reset();
  if (status != MS8607_status_ok)
    return status;

  return MS8607_status_ok;
}

/*
  \brief Set Humidity sensor ADC resolution.

  \param[in] MS8607_i2c_master_mode : I2C mode

  \return MS8607_status : status of MS8607
        - MS8607_status_ok
*/
void MS8607::set_humidity_i2c_master_mode(
    enum MS8607_humidity_i2c_master_mode mode)
{
  hsensor_i2c_master_mode = mode;
}

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
enum MS8607_status MS8607::get_battery_status(enum MS8607_battery_status *bat)
{
  enum MS8607_status status;
  uint8_t reg_value;

  status = hsensor_read_user_register(&reg_value);
  if (status != MS8607_status_ok)
    return status;

  if (reg_value & HSENSOR_USER_REG_END_OF_BATTERY_VDD_BELOW_2_25V)
    *bat = MS8607_battery_low;
  else
    *bat = MS8607_battery_ok;

  return status;
}

/*
  \brief Enable heater

  \return MS8607_status : status of MS8607
        - MS8607_status_ok : I2C transfer completed successfully
        - MS8607_status_i2c_transfer_error : Problem with i2c transfer
        - MS8607_status_no_i2c_acknowledge : I2C did not acknowledge
*/
enum MS8607_status MS8607::enable_heater(void)
{
  enum MS8607_status status;
  uint8_t reg_value;

  status = hsensor_read_user_register(&reg_value);
  if (status != MS8607_status_ok)
    return status;

  // Clear the resolution bits
  reg_value |= HSENSOR_USER_REG_ONCHIP_HEATER_ENABLE;
  hsensor_heater_on = true;

  status = hsensor_write_user_register(reg_value);

  return status;
}

/*
  \brief Disable heater

  \return MS8607_status : status of MS8607
        - MS8607_status_ok : I2C transfer completed successfully
        - MS8607_status_i2c_transfer_error : Problem with i2c transfer
        - MS8607_status_no_i2c_acknowledge : I2C did not acknowledge
*/
enum MS8607_status MS8607::disable_heater(void)
{
  enum MS8607_status status;
  uint8_t reg_value;

  status = hsensor_read_user_register(&reg_value);
  if (status != MS8607_status_ok)
    return status;

  // Clear the resolution bits
  reg_value &= ~HSENSOR_USER_REG_ONCHIP_HEATER_ENABLE;
  hsensor_heater_on = false;

  status = hsensor_write_user_register(reg_value);

  return status;
}

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
enum MS8607_status
MS8607::get_heater_status(enum MS8607_heater_status *heater)
{
  enum MS8607_status status;
  uint8_t reg_value;

  status = hsensor_read_user_register(&reg_value);
  if (status != MS8607_status_ok)
    return status;

  // Get the heater enable bit in reg_value
  if (reg_value & HSENSOR_USER_REG_ONCHIP_HEATER_ENABLE)
    *heater = MS8607_heater_on;
  else
    *heater = MS8607_heater_off;

  return status;
}

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
enum MS8607_status
MS8607::read_temperature_pressure_humidity(float *t, float *p, float *h)
{
  enum MS8607_status status;

  status = psensor_read_pressure_and_temperature(t, p);
  if (status != MS8607_status_ok)
    return status;

  status = hsensor_read_relative_humidity(h);
  if (status != MS8607_status_ok)
    return status;

  return MS8607_status_ok;
}

/******************** Functions from humidity sensor ********************/

/*
  \brief Check whether humidity sensor is connected

  \return bool : status of humidity sensor
        - true : Device is present
        - false : Device is not acknowledging I2C address
*/
bool MS8607::hsensor_is_connected(void)
{
#ifdef HIMAX_MS8607
  return true;
#else
  _i2cPort->beginTransmission((uint8_t)MS8607_HSENSOR_ADDR);
  return (_i2cPort->endTransmission() == 0);
#endif
}

/*
  \brief Reset the humidity sensor part

  \return MS8607_status : status of MS8607
        - MS8607_status_ok : I2C transfer completed successfully
        - MS8607_status_i2c_transfer_error : Problem with i2c transfer
        - MS8607_status_no_i2c_acknowledge : I2C did not acknowledge
*/
enum MS8607_status MS8607::hsensor_reset(void)
{
#ifdef HIMAX_MS8607
  uint8_t regAddr[1] = {(uint8_t)HSENSOR_RESET_COMMAND};
  if (hx_drv_i2cm_set_data((uint8_t)MS8607_HSENSOR_ADDR, regAddr, 1, 0, 0) != HX_DRV_LIB_PASS)
  {
    return MS8607_status_i2c_transfer_error;
  }

  hsensor_conversion_time = HSENSOR_CONVERSION_TIME_12b;
  hx_util_delay_ms(HSENSOR_RESET_TIME);
#else
  enum MS8607_status status;
  _i2cPort->beginTransmission((uint8_t)MS8607_HSENSOR_ADDR);
  _i2cPort->write((uint8_t)HSENSOR_RESET_COMMAND);
  _i2cPort->endTransmission();

  if (status != MS8607_status_ok)
    return status;

  hsensor_conversion_time = HSENSOR_CONVERSION_TIME_12b;
  delay(HSENSOR_RESET_TIME);
#endif

  return MS8607_status_ok;
}

/*
  \brief Check CRC

  \param[in] uint16_t : variable on which to check CRC
  \param[in] uint8_t : CRC value

  \return MS8607_status : status of MS8607
        - MS8607_status_ok : CRC check is OK
        - MS8607_status_crc_error : CRC check error
*/
enum MS8607_status MS8607::hsensor_crc_check(uint16_t value, uint8_t crc)
{
  uint32_t polynom = 0x988000; // x^8 + x^5 + x^4 + 1
  uint32_t msb = 0x800000;
  uint32_t mask = 0xFF8000;
  uint32_t result = (uint32_t)value << 8; // Pad with zeros as specified in spec

  while (msb != 0x80)
  {

    // Check if msb of current value is 1 and apply XOR mask
    if (result & msb)
      result = ((result ^ polynom) & mask) | (result & ~mask);

    // Shift by one
    msb >>= 1;
    mask >>= 1;
    polynom >>= 1;
  }
  if (result == crc)
    return MS8607_status_ok;
  else
    return MS8607_status_crc_error;
}

/*
  \brief Reads the MS8607 humidity user register.

  \param[out] uint8_t* : Storage of user register value

  \return MS8607_status : status of MS8607
        - MS8607_status_ok : I2C transfer completed successfully
        - MS8607_status_i2c_transfer_error : Problem with i2c transfer
        - MS8607_status_no_i2c_acknowledge : I2C did not acknowledge
*/
enum MS8607_status MS8607::hsensor_read_user_register(uint8_t *value)
{
  uint8_t buffer[1];
  buffer[0] = 0;
// Send the Read Register Command
#ifdef HIMAX_MS8607
  uint8_t regAddr[1] = {(uint8_t)HSENSOR_READ_USER_REG_COMMAND};
  if (hx_drv_i2cm_set_data((uint8_t)MS8607_HSENSOR_ADDR, regAddr, 1, 0, 0) != HX_DRV_LIB_PASS)
  {
    return MS8607_status_i2c_transfer_error;
  }
  if (hx_drv_i2cm_get_data((uint8_t)MS8607_HSENSOR_ADDR, NULL, 0, buffer, 1) != HX_DRV_LIB_PASS)
  {
    return MS8607_status_i2c_transfer_error;
  }
#else
  enum MS8607_status status;
  uint8_t i2c_status;
  _i2cPort->beginTransmission((uint8_t)MS8607_HSENSOR_ADDR);
  _i2cPort->write(HSENSOR_READ_USER_REG_COMMAND);
  i2c_status = _i2cPort->endTransmission();

  _i2cPort->requestFrom((uint8_t)MS8607_HSENSOR_ADDR, 1U);
  buffer[0] = _i2cPort->read();

  if (status != MS8607_status_ok)
    return status;

  if (i2c_status == i2c_status_err_overflow)
    return MS8607_status_no_i2c_acknowledge;
  if (i2c_status != i2c_status_ok)
    return MS8607_status_i2c_transfer_error;
#endif

  *value = buffer[0];

  return MS8607_status_ok;
}

/*
  \brief Writes the MS8607 humidity user register with value
         Will read and keep the unreserved bits of the register

  \param[in] uint8_t : Register value to be set.

  \return MS8607_status : status of MS8607
        - MS8607_status_ok : I2C transfer completed successfully
        - MS8607_status_i2c_transfer_error : Problem with i2c transfer
        - MS8607_status_no_i2c_acknowledge : I2C did not acknowledge
*/
enum MS8607_status MS8607::hsensor_write_user_register(uint8_t value)
{
  enum MS8607_status status;
  uint8_t reg;
  uint8_t data[2];

  status = hsensor_read_user_register(&reg);
  if (status != MS8607_status_ok)
    return status;

  // Clear bits of reg that are not reserved
  reg &= HSENSOR_USER_REG_RESERVED_MASK;
  // Set bits from value that are not reserved
  reg |= (value & ~HSENSOR_USER_REG_RESERVED_MASK);

  data[0] = HSENSOR_WRITE_USER_REG_COMMAND;
  data[1] = reg;

#ifdef HIMAX_MS8607
  uint8_t regAddr[1] = {(uint8_t)HSENSOR_WRITE_USER_REG_COMMAND};
  if (hx_drv_i2cm_set_data((uint8_t)MS8607_HSENSOR_ADDR, regAddr, 1, &reg, 1) != HX_DRV_LIB_PASS)
  {
    return MS8607_status_i2c_transfer_error;
  }
#else
  uint8_t i2c_status;
  _i2cPort->beginTransmission((uint8_t)MS8607_HSENSOR_ADDR);
  _i2cPort->write(HSENSOR_WRITE_USER_REG_COMMAND);
  _i2cPort->write(reg);
  i2c_status = _i2cPort->endTransmission();

  /* Do the transfer */
  if (i2c_status == i2c_status_err_overflow)
    return MS8607_status_no_i2c_acknowledge;
  if (i2c_status != i2c_status_ok)
    return MS8607_status_i2c_transfer_error;
#endif

  return MS8607_status_ok;
}

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
MS8607::set_humidity_resolution(enum MS8607_humidity_resolution res)
{
  enum MS8607_status status;
  uint8_t reg_value, tmp = 0;
  uint32_t conversion_time = HSENSOR_CONVERSION_TIME_12b;

  if (res == MS8607_humidity_resolution_12b)
  {
    tmp = HSENSOR_USER_REG_RESOLUTION_12b;
    conversion_time = HSENSOR_CONVERSION_TIME_12b;
  }
  else if (res == MS8607_humidity_resolution_10b)
  {
    tmp = HSENSOR_USER_REG_RESOLUTION_10b;
    conversion_time = HSENSOR_CONVERSION_TIME_10b;
  }
  else if (res == MS8607_humidity_resolution_8b)
  {
    tmp = HSENSOR_USER_REG_RESOLUTION_8b;
    conversion_time = HSENSOR_CONVERSION_TIME_8b;
  }
  else if (res == MS8607_humidity_resolution_11b)
  {
    tmp = HSENSOR_USER_REG_RESOLUTION_11b;
    conversion_time = HSENSOR_CONVERSION_TIME_11b;
  }

  status = hsensor_read_user_register(&reg_value);
  if (status != MS8607_status_ok)
    return status;

  // Clear the resolution bits
  reg_value &= ~HSENSOR_USER_REG_RESOLUTION_MASK;
  reg_value |= tmp & HSENSOR_USER_REG_RESOLUTION_MASK;

  hsensor_conversion_time = conversion_time;

  status = hsensor_write_user_register(reg_value);

  return status;
}

/*
  \brief Reads the relative humidity ADC value

  \param[out] uint16_t* : Relative humidity ADC value.

  \return MS8607_status : status of MS8607
        - MS8607_status_ok : I2C transfer completed successfully
        - MS8607_status_i2c_transfer_error : Problem with i2c transfer
        - MS8607_status_no_i2c_acknowledge : I2C did not acknowledge
        - MS8607_status_crc_error : CRC check error
*/
enum MS8607_status
MS8607::hsensor_humidity_conversion_and_read_adc(uint16_t *adc)
{
  enum MS8607_status status = MS8607_status_ok;
  uint16_t _adc;
  uint8_t buffer[3];
  uint8_t crc;

/* Read data */
#ifdef HIMAX_MS8607
  HX_DRV_ERROR_E hx_i2c_status;
  if (hsensor_i2c_master_mode == MS8607_i2c_hold)
  {
    uint8_t regAddr[1] = {(uint8_t)HSENSOR_READ_HUMIDITY_W_HOLD_COMMAND};
    hx_i2c_status = hx_drv_i2cm_set_data((uint8_t)MS8607_HSENSOR_ADDR, regAddr, 1, 0, 0);
  }
  else
  {
    uint8_t regAddr[1] = {(uint8_t)HSENSOR_READ_HUMIDITY_WO_HOLD_COMMAND};
    hx_i2c_status = hx_drv_i2cm_set_data((uint8_t)MS8607_HSENSOR_ADDR, regAddr, 1, 0, 0);
    // delay depending on resolution
    hx_util_delay_ms(hsensor_conversion_time);
  }

  if (hx_i2c_status != HX_DRV_LIB_PASS)
  {
    return MS8607_status_i2c_transfer_error;
  }

  if (hx_drv_i2cm_get_data((uint8_t)MS8607_HSENSOR_ADDR, NULL, 0, buffer, 3) != HX_DRV_LIB_PASS)
  {
    return MS8607_status_i2c_transfer_error;
  }
#else
  uint8_t i2c_status;
  uint8_t i;
  _i2cPort->beginTransmission((uint8_t)MS8607_HSENSOR_ADDR);

  if (hsensor_i2c_master_mode == MS8607_i2c_hold)
  {
    _i2cPort->write(HSENSOR_READ_HUMIDITY_W_HOLD_COMMAND);
    i2c_status = _i2cPort->endTransmission();
  }
  else
  {
    _i2cPort->write(HSENSOR_READ_HUMIDITY_WO_HOLD_COMMAND);
    i2c_status = _i2cPort->endTransmission();
    // delay depending on resolution
    delay(hsensor_conversion_time);
  }
  _i2cPort->requestFrom((uint8_t)MS8607_HSENSOR_ADDR, 3U);
  for (i = 0; i < 3; i++)
  {
    buffer[i] = _i2cPort->read();
  }

  if (status != MS8607_status_ok)
    return status;

  if (i2c_status == i2c_status_err_overflow)
    return MS8607_status_no_i2c_acknowledge;
  if (i2c_status != i2c_status_ok)
    return MS8607_status_i2c_transfer_error;
#endif

  _adc = (buffer[0] << 8) | buffer[1];
  crc = buffer[2];

  // compute CRC
  status = hsensor_crc_check(_adc, crc);
  if (status != MS8607_status_ok)
    return status;

  *adc = _adc;

  return status;
}

/*
  \brief Reads the relative humidity value.

  \param[out] float* : %RH Relative Humidity value

  \return MS8607_status : status of MS8607
        - MS8607_status_ok : I2C transfer completed successfully
        - MS8607_status_i2c_transfer_error : Problem with i2c transfer
        - MS8607_status_no_i2c_acknowledge : I2C did not acknowledge
        - MS8607_status_crc_error : CRC check error
*/
enum MS8607_status MS8607::hsensor_read_relative_humidity(float *humidity)
{
  enum MS8607_status status;
  uint16_t adc;

  status = hsensor_humidity_conversion_and_read_adc(&adc);
  if (status != MS8607_status_ok)
    return status;

  // Perform conversion function
  *humidity =
      (float)adc * HUMIDITY_COEFF_MUL / (1UL << 16) + HUMIDITY_COEFF_ADD;

  return status;
}

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
enum MS8607_status
MS8607::get_compensated_humidity(float temperature, float relative_humidity,
                                 float *compensated_humidity)
{
  if (hsensor_heater_on)
    return MS8607_status_heater_on_error;

  *compensated_humidity =
      (relative_humidity +
       (25 - temperature) * HSENSOR_TEMPERATURE_COEFFICIENT);

  return MS8607_status_ok;
}

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
enum MS8607_status MS8607::get_dew_point(float temperature,
                                         float relative_humidity,
                                         float *dew_point)
{
  double partial_pressure;

  if (hsensor_heater_on)
    return MS8607_status_heater_on_error;

  // Missing power of 10
  partial_pressure =
      pow(10, HSENSOR_CONSTANT_A -
                  HSENSOR_CONSTANT_B / (temperature + HSENSOR_CONSTANT_C));

  *dew_point =
      -HSENSOR_CONSTANT_B / (log10(relative_humidity * partial_pressure / 100) -
                             HSENSOR_CONSTANT_A) -
      HSENSOR_CONSTANT_C;

  return MS8607_status_ok;
}

/******************** Functions from Pressure sensor ********************/

/*
  \brief Check whether MS8607 pressure sensor is connected

  \return bool : status of MS8607
        - true : Device is present
        - false : Device is not acknowledging I2C address
*/
bool MS8607::psensor_is_connected(void)
{
#ifdef HIMAX_MS8607
  return true;
#else
  _i2cPort->beginTransmission((uint8_t)MS8607_PSENSOR_ADDR);
  return (_i2cPort->endTransmission() == 0);
#endif
}

/*
  \brief Reset the Pressure sensor part

  \return MS8607_status : status of MS8607
        - MS8607_status_ok : I2C transfer completed successfully
        - MS8607_status_i2c_transfer_error : Problem with i2c transfer
        - MS8607_status_no_i2c_acknowledge : I2C did not acknowledge
*/
enum MS8607_status MS8607::psensor_reset(void)
{
#ifdef HIMAX_MS8607
  uint8_t regAddr[1] = {(uint8_t)PSENSOR_RESET_COMMAND};
  if (hx_drv_i2cm_set_data((uint8_t)MS8607_PSENSOR_ADDR, regAddr, 1, 0, 0) != HX_DRV_LIB_PASS)
  {
    return MS8607_status_i2c_transfer_error;
  }
#else
  _i2cPort->beginTransmission((uint8_t)MS8607_PSENSOR_ADDR);
  _i2cPort->write(PSENSOR_RESET_COMMAND);
  _i2cPort->endTransmission();
#endif
  return MS8607_status_ok;
}

/*
  \brief CRC check

  \param[in] uint16_t *: List of EEPROM coefficients
  \param[in] uint8_t : crc to compare

  \return bool : TRUE if CRC is OK, FALSE if KO
*/
bool MS8607::psensor_crc_check(uint16_t *n_prom, uint8_t crc)
{
  uint8_t cnt, n_bit;
  uint16_t n_rem, crc_read;

  n_rem = 0x00;
  crc_read = n_prom[0];
  n_prom[COEFFICIENT_NUMBERS] = 0;
  n_prom[0] = (0x0FFF & (n_prom[0])); // Clear the CRC byte

  for (cnt = 0; cnt < (COEFFICIENT_NUMBERS + 1) * 2; cnt++)
  {

    // Get next byte
    if (cnt % 2 == 1)
      n_rem ^= n_prom[cnt >> 1] & 0x00FF;
    else
      n_rem ^= n_prom[cnt >> 1] >> 8;

    for (n_bit = 8; n_bit > 0; n_bit--)
    {

      if (n_rem & 0x8000)
        n_rem = (n_rem << 1) ^ 0x3000;
      else
        n_rem <<= 1;
    }
  }
  n_rem >>= 12;
  n_prom[0] = crc_read;

  return (n_rem == crc);
}

/*
  \brief Set pressure ADC resolution.

  \param[in] MS8607_pressure_resolution : Resolution requested

*/
void MS8607::set_pressure_resolution(enum MS8607_pressure_resolution res)
{
  psensor_resolution_osr = res;
}

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
enum MS8607_status MS8607::psensor_read_eeprom_coeff(uint8_t command,
                                                     uint16_t *coeff)
{
  uint8_t buffer[2];
  /* Read data */
#ifdef HIMAX_MS8607
  uint8_t regAddr[1] = {(uint8_t)command};
  if (hx_drv_i2cm_set_data((uint8_t)MS8607_PSENSOR_ADDR, regAddr, 1, 0, 0) != HX_DRV_LIB_PASS)
  {
    return MS8607_status_i2c_transfer_error;
  }

  if (hx_drv_i2cm_get_data((uint8_t)MS8607_PSENSOR_ADDR, NULL, 0, buffer, 2) != HX_DRV_LIB_PASS)
  {
    return MS8607_status_i2c_transfer_error;
  }

#else
  enum MS8607_status status;
  uint8_t i2c_status;
  uint8_t i;
  _i2cPort->beginTransmission((uint8_t)MS8607_PSENSOR_ADDR);
  _i2cPort->write(command);
  i2c_status = _i2cPort->endTransmission();

  _i2cPort->requestFrom((uint8_t)MS8607_PSENSOR_ADDR, 2U);
  for (i = 0; i < 2; i++)
    buffer[i] = _i2cPort->read();

  if (status != MS8607_status_ok)
    return status;

  if (i2c_status == i2c_status_err_overflow)
    return MS8607_status_no_i2c_acknowledge;
  if (i2c_status != i2c_status_ok)
    return MS8607_status_i2c_transfer_error;
#endif

  *coeff = (buffer[0] << 8) | buffer[1];

  if (*coeff == 0)
    return MS8607_status_i2c_transfer_error;

  return MS8607_status_ok;
}

/*
  \brief Reads the MS8607 EEPROM coefficients to store them for computation.

  \return MS8607_status : status of MS8607
        - MS8607_status_ok : I2C transfer completed successfully
        - MS8607_status_i2c_transfer_error : Problem with i2c transfer
        - MS8607_status_no_i2c_acknowledge : I2C did not acknowledge
        - MS8607_status_crc_error : CRC check error on the coefficients
*/
enum MS8607_status MS8607::psensor_read_eeprom(void)
{
  enum MS8607_status status;
  uint8_t i;

  for (i = 0; i < COEFFICIENT_NUMBERS; i++)
  {
    status = psensor_read_eeprom_coeff(PROM_ADDRESS_READ_ADDRESS_0 + i * 2,
                                       eeprom_coeff + i);
    if (status != MS8607_status_ok)
      return status;
  }

  if (!psensor_crc_check(eeprom_coeff,
                         (eeprom_coeff[CRC_INDEX] & 0xF000) >> 12))
    return MS8607_status_crc_error;

  return MS8607_status_ok;
}

/*
  \brief Triggers conversion and read ADC value

  \param[in] uint8_t : Command used for conversion (will determine Temperature
  vs Pressure and osr)
  \param[out] uint32_t* : ADC value.

  \return MS8607_status : status of MS8607
        - MS8607_status_ok : I2C transfer completed successfully
        - MS8607_status_i2c_transfer_error : Problem with i2c transfer
        - MS8607_status_no_i2c_acknowledge : I2C did not acknowledge
*/
enum MS8607_status MS8607::psensor_conversion_and_read_adc(uint8_t cmd,
                                                           uint32_t *adc)
{
  uint8_t buffer[3];
/* Read data */
#ifdef HIMAX_MS8607
  uint8_t regAddr[1] = {(uint8_t)cmd};
  if (hx_drv_i2cm_set_data((uint8_t)MS8607_PSENSOR_ADDR, regAddr, 1, 0, 0) != HX_DRV_LIB_PASS)
  {
    return MS8607_status_i2c_transfer_error;
  }
  // 20ms wait for conversion
  //delay(psensor_conversion_time[(cmd & PSENSOR_CONVERSION_OSR_MASK) / 2]);
  hx_util_delay_ms(psensor_conversion_time[psensor_resolution_osr]);

  // Send the read command
  regAddr[0] = (uint8_t)PSENSOR_READ_ADC;
  if (hx_drv_i2cm_set_data((uint8_t)MS8607_PSENSOR_ADDR, regAddr, 1, 0, 0) != HX_DRV_LIB_PASS)
  {
    return MS8607_status_i2c_transfer_error;
  }
  if (hx_drv_i2cm_get_data((uint8_t)MS8607_PSENSOR_ADDR, NULL, 0, buffer, 3) != HX_DRV_LIB_PASS)
  {
    return MS8607_status_i2c_transfer_error;
  }
#else
  enum MS8607_status status;
  HX_DRV_ERROR_E i2c_status;
  uint8_t i;
  _i2cPort->beginTransmission((uint8_t)MS8607_PSENSOR_ADDR);
  _i2cPort->write(cmd);
  i2c_status = _i2cPort->endTransmission();

  // 20ms wait for conversion
  //delay(psensor_conversion_time[(cmd & PSENSOR_CONVERSION_OSR_MASK) / 2]);
  delay(psensor_conversion_time[psensor_resolution_osr]);

  // Send the read command
  _i2cPort->beginTransmission((uint8_t)MS8607_PSENSOR_ADDR);
  _i2cPort->write((uint8_t)PSENSOR_READ_ADC);
  i2c_status = _i2cPort->endTransmission();

  _i2cPort->requestFrom((uint8_t)MS8607_PSENSOR_ADDR, 3U);
  for (i = 0; i < 3; i++)
    buffer[i] = _i2cPort->read();

  if (status != MS8607_status_ok)
    return status;

  if (status != MS8607_status_ok)
    return status;

  if (i2c_status == i2c_status_err_overflow)
    return MS8607_status_no_i2c_acknowledge;
  if (i2c_status != i2c_status_ok)
    return MS8607_status_i2c_transfer_error;
#endif

  *adc = ((uint32_t)buffer[0] << 16) | ((uint32_t)buffer[1] << 8) | buffer[2];

  return MS8607_status_ok;
}

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
enum MS8607_status
MS8607::psensor_read_pressure_and_temperature(float *temperature,
                                              float *pressure)
{
  enum MS8607_status status = MS8607_status_ok;
  uint32_t adc_temperature, adc_pressure;
  int32_t dT, TEMP;
  int64_t OFF, SENS, P, T2, OFF2, SENS2;
  uint8_t cmd;

  // First read temperature
  cmd = psensor_resolution_osr * 2;
  cmd |= PSENSOR_START_TEMPERATURE_ADC_CONVERSION;

  status = psensor_conversion_and_read_adc(cmd, &adc_temperature);
  if (status != MS8607_status_ok)
    return status;
  // Now read pressure
  cmd = psensor_resolution_osr * 2;
  cmd |= PSENSOR_START_PRESSURE_ADC_CONVERSION;
  status = psensor_conversion_and_read_adc(cmd, &adc_pressure);
  if (status != MS8607_status_ok)
    return status;
  if (adc_temperature == 0 || adc_pressure == 0)
    return MS8607_status_i2c_transfer_error;
  // Difference between actual and reference temperature = D2 - Tref
  dT = (int32_t)adc_temperature -
       ((int32_t)eeprom_coeff[REFERENCE_TEMPERATURE_INDEX] << 8);

  // Actual temperature = 2000 + dT * TEMPSENS
  TEMP = 2000 + ((int64_t)dT *
                     (int64_t)eeprom_coeff[TEMP_COEFF_OF_TEMPERATURE_INDEX] >>
                 23);

  // Second order temperature compensation
  if (TEMP < 2000)
  {
    T2 = (3 * ((int64_t)dT * (int64_t)dT)) >> 33;
    OFF2 = 61 * ((int64_t)TEMP - 2000) * ((int64_t)TEMP - 2000) / 16;
    SENS2 = 29 * ((int64_t)TEMP - 2000) * ((int64_t)TEMP - 2000) / 16;

    if (TEMP < -1500)
    {
      OFF2 += 17 * ((int64_t)TEMP + 1500) * ((int64_t)TEMP + 1500);
      SENS2 += 9 * ((int64_t)TEMP + 1500) * ((int64_t)TEMP + 1500);
    }
  }
  else
  {
    T2 = (5 * ((int64_t)dT * (int64_t)dT)) >> 38;
    OFF2 = 0;
    SENS2 = 0;
  }

  // OFF = OFF_T1 + TCO * dT
  OFF = ((int64_t)(eeprom_coeff[PRESSURE_OFFSET_INDEX]) << 17) +
        (((int64_t)(eeprom_coeff[TEMP_COEFF_OF_PRESSURE_OFFSET_INDEX]) * dT) >>
         6);
  OFF -= OFF2;

  // Sensitivity at actual temperature = SENS_T1 + TCS * dT
  SENS =
      ((int64_t)eeprom_coeff[PRESSURE_SENSITIVITY_INDEX] << 16) +
      (((int64_t)eeprom_coeff[TEMP_COEFF_OF_PRESSURE_SENSITIVITY_INDEX] * dT) >>
       7);
  SENS -= SENS2;

  // Temperature compensated pressure = D1 * SENS - OFF
  P = (((adc_pressure * SENS) >> 21) - OFF) >> 15;

  *temperature = ((float)TEMP - T2) / 100;
  *pressure = (float)P / 100;

  return MS8607_status_ok;
}

//Returns the latest pressure reading. Will initiate a reading if data is expired
float MS8607::getPressure()
{
  if (pressureHasBeenRead == true)
  {
    //Get a new reading
    read_temperature_pressure_humidity(&globalTemperature, &globalPressure, &globalHumidity);
    temperatureHasBeenRead = false;
    humidityHasBeenRead = false;
  }
  pressureHasBeenRead = true;
  return (globalPressure);
}

//Returns the latest temp reading. Will initiate a reading if data is expired
float MS8607::getTemperature()
{
  if (temperatureHasBeenRead == true)
  {
    //Get a new reading
    read_temperature_pressure_humidity(&globalTemperature, &globalPressure, &globalHumidity);
    pressureHasBeenRead = false;
    humidityHasBeenRead = false;
  }
  temperatureHasBeenRead = true;
  return (globalTemperature);
}

//Returns the latest humidity reading. Will initiate a reading if data is expired
float MS8607::getHumidity()
{
  if (humidityHasBeenRead == true)
  {
    //Get a new reading
    read_temperature_pressure_humidity(&globalTemperature, &globalPressure, &globalHumidity);
    pressureHasBeenRead = false;
    temperatureHasBeenRead = false;
  }
  humidityHasBeenRead = true;
  return (globalHumidity);
}

// Given a pressure P (mb) taken at a specific altitude (meters),
// return the equivalent pressure (mb) at sea level.
// This produces pressure readings that can be used for weather measurements.
// Returns pressure in mb
double MS8607::adjustToSeaLevel(double absolutePressure, double actualAltitude)
{
  return (absolutePressure / pow(1 - (actualAltitude / 44330.0), 5.255));
}

// Given a pressure measurement (mb) and the pressure at a baseline (mb),
// return altitude change (in meters) for the delta in pressures.
double MS8607::altitudeChange(double currentPressure, double baselinePressure)
{
  return (44330.0 * (1 - pow(currentPressure / baselinePressure, 1 / 5.255)));
}
