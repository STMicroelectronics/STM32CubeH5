## <b>PSSI_Transmit_Receive_DMA Example Description</b>

This example describes how to perform PSSI data buffer transmission/reception between a slave configured on one board and a master simulated by another board.

This project is configured for STM32H5E5xx devices using STM32CubeH5 HAL and runs on the NUCLEO-H5E5ZJ board from STMicroelectronics.

**Board:** NUCLEO-H5E5ZJ

```
D0  Pin: PE12  (CN10 PIN11)
D1  Pin: PA10  (CN10 PIN33)
D2  Pin: PB15  (CN10 PIN26)
D3  Pin: PC9   (CN10 PIN1)
D4  Pin: PC11  (CN7  PIN2)
D5  Pin: PD3   (CN10 PIN29)
D6  Pin: PF4   (CN10 PIN19)
D7  Pin: PB4   (CN10 PIN35)
Clk Pin: PA6   (CN7  PIN28)
DE  Pin: PA4   (CN7  PIN32)
RDY Pin: PE14  (CN10 PIN15)
TIM_PWM: PE9   (CN10 PIN23)
```


The PSSI communication is as shown below:

```
  _________________________                       _________________________
 |           ______________|                     |______________           |
 |          |PSSI          |                     |          PSSI|          |
 |          |              |                     |              |          |
 |          |          D0  |_____________________| D0           |          |
 |          |           .  |_____________________| .            |          |
 |          |           .  |_____________________| .            |          |
 |          |           .  |_____________________| .            |          |
 |          |          D7  |                     | D7           |          |
 |          |          Clk |_____________________| Clk _________|__        |
 |          |          DE  |_____________________| DE           |  |       |
 |          |              |                     |              |  |       |
 |          |              |                     |              |  |       |
 |          |              |                     |       TIM_PWM|__|       |
 |          |              |                     |              |          |
 |          |______________|                     |______________|          |
 |                         |                     |                         |
 |                      GND|_____________________|GND                      |
 |_STM32_Board Slave ______|                     |_STM32_Board Master______|
```


At the beginning of the main program, the `HAL_Init()` function is called to reset all peripherals, initialize the Flash interface, and the systick.
The `SystemClock_Config()` function is used to configure the system clock for STM32H5E5xx devices.


The PSSI peripheral configuration is ensured by the `HAL_PSSI_Init()` function.
This calls the `HAL_PSSI_MspInit()` function, which implements the configuration of the needed PSSI resources according to the hardware (CLOCK, GPIO, DMA, and NVIC). You may update this function to change PSSI configuration.



**Transmission/Reception Process (Slave):**

- The `HAL_PSSI_Transmit_DMA()` and `HAL_PSSI_Receive_DMA()` functions allow, respectively, the transmission and reception of a predefined data buffer once the clock is ON in the specified Pin (PA6).


  1. Transmit the data
  2. Receive the data
  3. Verify that the received data matches the sent one and is modified by the "Master"



  NUCLEO-H5E5ZJ board's LEDs can be used to monitor the transfer status:
  - **LED1 (GREEN):** ON when reception is ongoing.
  - **LED3 (BLUE):** ON when transmission is ongoing.
  - **LED1 (GREEN):** BLINKS when transmission/reception are done.
  - **LED2 (RED):** ON if there are errors while receiving/transmitting data.


**Master Simulation Process:**

- TIM1 generates the clock to allow the slave to transmit and receive data.
  1. Receives the data from the PSSI_Slave ("Hello from Slave")
  2. Verifies that it is a request from a Slave
  3. Modifies it by adding ": received, from Master"
  4. Transmits "Hello from Slave: received, from Master"

  NUCLEO-H5E5ZJ board's LEDs can be used to monitor the transfer status:
  - **LED1 (GREEN):** ON when reception is ongoing.
  - **LED3 (BLUE):** ON when transmission is ongoing.
  - **LED1 (GREEN):** BLINKS when transmission/reception are done.
  - **LED2 (RED):** ON if there are errors while receiving/transmitting data.



The user can choose between Master and Slave through `#define MASTER_BOARD` in the `main.c` file:
- If the Master board is used, `#define MASTER_BOARD` must be uncommented.
- If the Slave board is used, `#define MASTER_BOARD` must be commented.


#### <b>Notes</b>


1. Care must be taken when using `HAL_Delay()`. This function provides accurate delay (in milliseconds) based on a variable incremented in the SysTick ISR. If `HAL_Delay()` is called from a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower) than the peripheral interrupt. Otherwise, the caller ISR process will be blocked. To change the SysTick interrupt priority, use the `HAL_NVIC_SetPriority()` function.
2. The example needs to ensure that the SysTick time base is always set to 1 millisecond to have correct HAL operation.


### <b>Keywords</b>

Transmission, Reception, PSSI, DMA, Slave,Clock

### <b>Directory contents</b>

    - PSSI/PSSI_Transmit_Receive_DMA/Inc/stm32h5xx_nucleo_conf.h     BSP configuration file
    - PSSI/PSSI_Transmit_Receive_DMA/Inc/stm32h5xx_hal_conf.h        HAL configuration file
    - PSSI/PSSI_Transmit_Receive_DMA/Inc/stm32h5xx_it.h              Interrupt handlers header file
    - PSSI/PSSI_Transmit_Receive_DMA/Inc/main.h                      Header for main.c module
    - PSSI/PSSI_Transmit_Receive_DMA/Src/stm32h5xx_it.c              Interrupt handlers
    - PSSI/PSSI_Transmit_Receive_DMA/Src/main.c                      Main program
    - PSSI/PSSI_Transmit_Receive_DMA/Src/stm32h5xx_hal_msp.c         HAL MSP module 
    - PSSI/PSSI_Transmit_Receive_DMA/Src/system_stm32h5xx.c          STM32H5xx system source file

     

### <b>Hardware and Software environment</b>

- This example runs on STM32H5E5ZJTx devices.
- It has been tested with an STMicroelectronics NUCLEO-H5E5ZJ board and can be easily tailored to any other supported device and development board.

**NUCLEO-H5E5ZJ Set-up:**
  - Connect PSSI Data lines, clock line, and Data Enable line of Master board to PSSI lines of Slave Board (described previously).
  - Connect PSSI clock line of Master board to TIM_PWM Pin of the same board (described previously).
  - Connect GND of Master board to GND of Slave Board.

## How to use it?

To make the program work, follow these steps:

1. Open your preferred toolchain.
2. Rebuild all files and load your image into target memory:
   - Uncomment `#define MASTER_BOARD` and load the project in Master Board.
   - Comment `#define MASTER_BOARD` and load the project in Slave Board.
3. Run the example.

