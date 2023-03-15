## <b>I2C_OneBoard_Communication_DMAAndIT_Init Example Description</b>

How to transmit data bytes from an I2C master device using DMA mode
to an I2C slave device using interrupt mode. The peripheral is initialized with
LL unitary service functions to optimize for performance and size.

This example guides you through the different configuration steps by mean of LL API
to configure GPIO, DMA and I2C peripherals using only one NUCLEO-H563ZI.

The user can disable internal pull-up by opening ioc file.
For that, user can follow the procedure :

 - Double click on the I2C_OneBoard_Communication_DMAAndIT_Init.ioc file
 - When CUBEMX tool is opened, select System Core category
 - Then in the configuration of GPIO/I2C1, change Pull-up to No pull-up and no pull-down for the both pins
 - Same action in the configuration of GPIO/I2C2, change Pull-up to No pull-up and no pull-down for the both pins
 - Last step, generate new code thanks to button "GENERATE CODE"

The example is updated with no pull on each pin used for I2C communication

 - I2C1 Peripheral is configured in Slave mode with EXTI (Clock 400Khz, Own address 7-bit enabled).
 - I2C2 Peripheral is configured in Master mode with DMA (Clock 400Khz).
 - GPIO associated to User push-button is linked with EXTI.

LED1 blinks quickly to wait for user-button press.

Example execution:

Press the USER push-button to initiate a write request by Master through Handle_I2C_Master() routine.
This action will generate an I2C start condition with the Slave address and a write bit condition.
When address Slave match code is received on I2C1, an ADDR interrupt occurs.
I2C1 IRQ Handler routine is then checking Address Match Code and direction Write.
This will allow Slave to enter in receiver mode and then acknowledge Master to send the bytes through DMA.
When acknowledge is received on I2C2, DMA transfer the data from flash memory buffer to I2C2 TXDR register.
This will allow Master to transmit a byte to the Slave.
Each time a byte is received on I2C1 (Slave), an RXNE interrupt occurs until a STOP condition.
And so each time the Slave acknowledge the byte received,
DMA transfer the next data from flash memory buffer to I2C2 TXDR register until Transfer completed.
Master auto-generate a Stop condition when DMA transfer is achieved.

The STOP condition generate a STOP interrupt and initiate the end of reception on Slave side.
I2C1 IRQ handler and Handle_I2C_Master() routine are then clearing the STOP flag in both side.

LED1 is On if data are well received.

In case of errors, LED1 is blinking slowly (1s).

### <b>Keywords</b>

Connectivity, Communication, I2C, DMA, Interrupt, Master, Slave, Transmission, Reception, Fast mode

### <b>Directory contents</b>

  - I2C/I2C_OneBoard_Communication_DMAAndIT_Init/Inc/stm32h5xx_it.h          Interrupt handlers header file
  - I2C/I2C_OneBoard_Communication_DMAAndIT_Init/Inc/main.h                  Header for main.c module
  - I2C/I2C_OneBoard_Communication_DMAAndIT_Init/Inc/stm32_assert.h          Template file to include assert_failed function
  - I2C/I2C_OneBoard_Communication_DMAAndIT_Init/Src/stm32h5xx_it.c          Interrupt handlers
  - I2C/I2C_OneBoard_Communication_DMAAndIT_Init/Src/main.c                  Main program
  - I2C/I2C_OneBoard_Communication_DMAAndIT_Init/Src/system_stm32h5xx.c      STM32H5xx system source file

### <b>Hardware and Software environment</b>

  - This example runs on STM32H563ZITx devices.

  - This example has been tested with NUCLEO-H563ZI board and can be
    easily tailored to any other supported device and development board.

  - NUCLEO-H563ZI Set-up
    - Connect GPIOs connected to I2C1 SCL/SDA (PB.8 and PB.9)
    to respectively SCL and SDA pins of I2C2 (PF.1 and PF.0).
      - I2C1_SCL  PB.8 (CN5, pin 3) : connected to I2C2_SCL PF.1 (CN9, pin 19)
      - I2C1_SDA  PB.9 (CN5, pin 5) : connected to I2C2_SDA PF.0 (CN9, pin 21)

  - Launch the program. Press USER push-button to initiate a write request by Master
      then Slave receive bytes.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

