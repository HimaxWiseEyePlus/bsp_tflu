# qwiic_ms8607 example
- Setting
    <p align="left">
    <img width="400" height="550" src="../../images/HIMAX_WE1_EVB_with_ms8607.png">
    </p>

- Build qwiic_ms8607 example and flash image. We provide 2 examples to read data from qwiic_ms8607, you can choose either of them.
  - Example with himax qwiic SDK, detail device initialization will be done by SDK, you can simply call initial and get_data function to retrieve the data. Flash image name will be `qwiic_ms8607.img`.
    ```bash
    make qwiic_ms8607
    make flash example=qwiic_ms8607
    ```
  - Example with himax i2c SDK, directly use SDK i2c master API to initial device and get data back, you can see more detail in the example. Flash image name will be `qwiic_ms8607_sparkfun.img`.
    ```bash
    make qwiic_ms8607_sparkfun
    make flash example=qwiic_ms8607_sparkfun
    ```

- After above steps, update `qwiic_ms8607.img` to HIMAX WE1 EVB. After get data from sensor, we can display them on the console.
    ![alt text](../../images/ms8607_console.png)