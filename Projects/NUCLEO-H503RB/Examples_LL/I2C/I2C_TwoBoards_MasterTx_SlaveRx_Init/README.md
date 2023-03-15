## <b>I2C_TwoBoards_MasterTx_SlaveRx_Init Example Description</b> 

How to transmit data bytes from an I2C master device using polling mode
to an I2C slave device using interrupt mode. The peripheral is initialized
with LL unitary service functions to optimize for performance and size.

This example guides you through the different configuration steps by mean of LL API
to configure GPIO and I2C peripherals using two NUCLEO-H503RB.

       Boards: NUCLEO-H503RB (embeds a STM32H503RB device)
       - SCL Pin: PB.8 (CN10, pin 36)
       - SDA Pin: PB.10 (CN10, pin 25)

The project is split in two parts the Master Board and the Slave Board

- Master Board :
  I2C1 Peripheral is configured in Master mode (Clock 400Khz).
  And GPIO associated to USER push-button is linked with EXTI.

- Slave Board :
  I2C1 Peripheral is configured in Slave mode (Clock 400Khz, Own address 7-bit enabled).

The user can choose between Master and Slave through "#define SLAVE_BOARD"
in the "main.h" file:

- Comment "#define SLAVE_BOARD" to select Master board.
- Uncomment "#define SLAVE_BOARD" to select Slave board.

The user can disable internal pull-up by opening ioc file.
For that, user can follow the procedure :

1- Double click on the I2C_TwoBoards_MasterTx_SlaveRx_Init.ioc file

2- When CUBEMX tool is opened, select System Core category

3- Then in the configuration of GPIO/I2C1, change Pull-up to No pull-up and no pull-down for the both pins

4- Last step, generate new code thanks to button "GENERATE CODE"

The example is updated with no pull on each pin used for I2C communication

A first program launch, BOARD SLAVE waiting Address Match code through Handle_I2C_Slave() routine.
LED2 blinks quickly on BOARD MASTER to wait for USER push-button press.

Example execution:

Press the USER push-button on BOARD MASTER to initiate a write request by Master through Handle_I2C_Master() routine.
This action will generate an I2C start condition with the Slave address and a write bit condition.
When address Slave match code is received on I2C1 of BOARD SLAVE, an ADDR event occurs.
Handle_I2C_Slave() routine is then checking Address Match Code and direction Write.
This will allow Slave to enter in receiver mode and then acknowledge Master to send the bytes.
When acknowledge is received on I2C1 (Master), a TXIS event occurs.
This will allow Master to transmit a byte to the Slave.
Each time a byte is received on I2C1 (Slave), an RXNE event occurs until a STOP condition.
And so each time the Slave acknowledge the byte received a TXIS event occurs on Master side.
Master auto-generate a Stop condition when size of data to transmit is achieved.

The STOP condition generate a STOP event and initiate the end of reception on Slave side.
Handle_I2C_Slave() and Handle_I2C_Master() routine are then clearing the STOP flag in both side.

LED2 is On :

- Slave side if data are well received.
- Master side if transfer sequence is completed.

In case of errors, LED2 is blinking slowly (1s).

### <b>Keywords</b>

Connectivity, Communication, I2C, Master Rx, Slave Tx, Transmission, Reception, Fast mode, SDA, SCL

### <b>Directory contents</b> 

  - I2C/I2C_TwoBoards_MasterTx_SlaveRx_Init/Inc/stm32h5xx_it.h          Interrupt handlers header file
  - I2C/I2C_TwoBoards_MasterTx_SlaveRx_Init/Inc/main.h                  Header for main.c module
  - I2C/I2C_TwoBoards_MasterTx_SlaveRx_Init/Inc/stm32_assert.h          Template file to include assert_failed function
  - I2C/I2C_TwoBoards_MasterTx_SlaveRx_Init/Src/stm32h5xx_it.c          Interrupt handlers
  - I2C/I2C_TwoBoards_MasterTx_SlaveRx_Init/Src/main.c                  Main program
  - I2C/I2C_TwoBoards_MasterTx_SlaveRx_Init/Src/system_stm32h5xx.c      STM32H5xx system source file

### <b>Hardware and Software environment</b>

  - This example runs on STM32H503RBTx devices.

  - This example has been tested with STM32H503RBTx board and can be
    easily tailored to any other supported device and development board.

  - NUCLEO-H503RB Set-up
    - Connect GPIOs connected to Board Slave I2C1 SCL/SDA (PB.8 and PB.10)
    to respectively Board Master SCL and SDA pins of I2C1 (PB.8 and PB.10).
      - I2C1_SCL  PB.8 (CN10, pin 36) : connected to I2C1_SCL PB.8 (CN10, pin 36)
      - I2C1_SDA  PB.10 (CN10, pin 25) : connected to I2C1_SDA PB.10 (CN10, pin 25)
    - Connect Master board GND to Slave Board GND

  - Launch the program.
  - Press USER push-button to initiate a write request by Master
      then Slave receive bytes.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory (The user can choose between Master
   and Slave target through "#define SLAVE_BOARD" in the "main.h" file)
    - Comment "#define SLAVE_BOARD" and load the project in Master Board
    - Uncomment "#define SLAVE_BOARD" and load the project in Slave Board
 - Run the example

