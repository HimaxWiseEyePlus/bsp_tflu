# qwiic_ccs811_bme280 example

- Setting
  <p align="left">
  <img width="300" height="550" src="../../images/HIMAX_WE1_EVB_with_bme280.jpg">
  </p>

- Build qwiic_ccs811_bme280 example and flash image, flash image name will be `qwiic_ccs811_bme280.img`.
  ```bash
  make qwiic_ccs811_bme280
  make flash example=qwiic_ccs811_bme280
  ```

- After above steps, update `qwiic_ccs811_bme280.img` to HIMAX WE1 EVB.After get data from sensor, we can display them on the console.

  ![alt text](../../images/bme280_console.png)
