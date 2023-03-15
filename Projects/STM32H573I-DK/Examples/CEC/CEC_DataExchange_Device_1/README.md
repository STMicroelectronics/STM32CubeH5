## <b>CEC_DataExchange_Device_1 Example Description</b>

This example shows how to configure and use the CEC peripheral to receive and 
transmit messages.

- Hardware Description

To use this example, two STM32H573I-DK boards (called Device_1 and 
Device_2) are loaded with the matching software then connected through CEC lines
(PB6 CN13 pin 10) and GND.

Development board = STM32H573I-DK

- Software Description

The test unrolls as follows.

On TX side, four possible messages can be transmitted and are indicated on the screen :

 - when touching top of the screen, send CEC command with opcode 0x44 and operand 0x41
 - when touching bottom of the screen, send CEC command with opcode 0x44 and operand 0x42
 - when touching left of the screen, send CEC command with opcode 0x46
 - when touching right of the screen, send CEC command with opcode 0x9F

On the RX side in case of successful reception, it displays the content of the received 
CEC command or displays an error message if transmission fails.

The two boards transmits commands to other by touching the screen and receives the 
commands from the other and displays the received command on the screen.

In this example, HCLK is configured at 250 MHz.

### <b>Keywords</b>

Connectivity, CEC, Transmission, Reception, Joystick, Data Exchange

### <b>Directory contents</b>

  - CEC/CEC_DataExchange_Device_1/Inc/stm32h5xx_hal_conf.h    HAL configuration file
  - CEC/CEC_DataExchange_Device_1/Inc/stm32h5xx_it.h          Interrupt handlers header file
  - CEC/CEC_DataExchange_Device_1/Inc/main.h                  Header for main.c module  
  - CEC/CEC_DataExchange_Device_1/Src/stm32h5xx_it.c          Interrupt handlers
  - CEC/CEC_DataExchange_Device_1/Src/system_stm32h5xx.c      STM32H5xx system source file
  - CEC/CEC_DataExchange_Device_1/Src/main.c                  Main program
  - CEC/CEC_DataExchange_Device_1/Src/stm32h5xx_hal_msp.c     IP hardware resources initialization


### <b>Hardware and Software environment</b>

  - This example runs on STM32H573IIKxQ devices.
    
  - This example has been tested with STM32H573I-DK board and can be
    easily tailored to any other supported device and development board.      

  - STM32H573I-DK Set-up
  
    - Connect CEC line of Device_1 (PB6 CN13 pin 10) to CEC line of Device_2 (PB6 CN13 pin 10).
    - Connect GND of Device_1 to GND of Device_2.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files (CEC_DataExchange_Device_1 project) and load your image into target memory
    o Load the project in Device_1 Board
 - Rebuild all files (CEC_DataExchange_Device_2 project) and load your image into target memory
    o Load the project in Device_2 Board
 - With a wire, connect PB6 between the 2 boards
 - Connect the ground of the 2 boards
 - Run the example


