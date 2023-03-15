## <b>LPTIM_PulseCounter_Init Example Description</b>

How to use the LPTIM peripheral in counter mode to generate a PWM output signal 
and update its duty cycle. This example is based on the STM32H5xx
LPTIM LL API. The peripheral is initialized with LL initialization 
function to demonstrate LL init usage.

To reduce power consumption, MCU enters stop mode after starting counting. Each
time the counter reaches the maximum value (Period/Autoreload), an interruption
is generated, the MCU is woken up from stop mode and LED2 toggles the last state.

In this example Period value is set to 1000, so each time the counter counts
(1000 + 1) rising edges on LPTIM1_IN1 pin, an interrupt is generated and LED2
toggles. If the external function generator is set to provide a square waveform at 1Khz,
the led will toggle each second.

In this example the internal clock provided to the LPTIM1 is LSI (32 kHz),
so the external input is sampled with LSI clock. In order not to miss any event,
the frequency of the changes on the external Input1 signal should never exceed the
frequency of the internal clock provided to the LPTIM1 (LSI for the
present example).


### <b>Keywords</b>

Timer, Low Power, Pulse Counter, Stop mode, Interrupt

### <b>Directory contents</b>

  - LPTIM/LPTIM_PulseCounter_Init/Inc/stm32h5xx_it.h          Interrupt handlers header file
  - LPTIM/LPTIM_PulseCounter_Init/Inc/main.h                  Header for main.c module
  - LPTIM/LPTIM_PulseCounter_Init/Inc/stm32_assert.h          Template file to include assert_failed function
  - LPTIM/LPTIM_PulseCounter_Init/Src/stm32h5xx_it.c          Interrupt handlers
  - LPTIM/LPTIM_PulseCounter_Init/Src/main.c                  Main program
  - LPTIM/LPTIM_PulseCounter_Init/Src/system_stm32h5xx.c      STM32H5xx system source file


### <b>Hardware and Software environment</b>

  - This example runs on STM32H503RBTx devices.
    
  - This example has been tested with NUCLEO-H503RB board and can be
    easily tailored to any other supported device and development board.
    
  - Connect a square waveform generator to PB5 (Arduino connector CN9 pin 5 D4, Morpho connector CN10 pin 29).
    If the frequency of the signal is 1 kHz, LED2 toggles every second.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

