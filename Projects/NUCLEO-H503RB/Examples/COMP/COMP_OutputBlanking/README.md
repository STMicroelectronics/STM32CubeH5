## <b>COMP_OutputBlanking Example Description</b>

How to use the comparator-peripheral output blanking feature. The purpose of the
output blanking feature in motor control is to prevent tripping of the current
regulation upon short current spikes at the beginning of the PWM period.

In this example COMP1 and TIM2 are used:

COMP1 is configured as following:

 - Non Inverting input is connected to PB0
 - Inverting input is connected to VREFINT (1.22V)
 - Output is available on PC5 (pin 6 on connector CN10)
 - Blanking source is connected internally to TIM2 TIM_CHANNEL_3

TIM2 is configured as following:

 - TIM2 frequency is 1kHz.
 - TIM2 TIM_CHANNEL_3  (PB10 - pin 7 on connector CN9) configured in PWM mode.
 - Timer channel intended to be connected to an external device (motor, ...) in user application:
   TIM2 TIM_CHANNEL_1 (PA0 - pin 1 on connector CN8) configured in PWM mode.
 - Both these PWM are running at same frequency of TIM2.

Connection needed:

 - either connect COMP1 input (PB0 - pin 4 on connector CN8) to TIM2 TIM_CHANNEL_1 (PA0 - pin 1 on connector CN8).
 - either connect COMP1 input (PB0 - pin 4 on connector CN8) to a signal generator, at the frequency of TIM2 (to have comparator input signal frequency matching blanking signal frequency).
   To do this, possibility to use TIM2 TIM_CHANNEL_1 (PA0 - pin 1 on connector CN8) as trigger of signal generator).
 
Comparator output signal expected:

 - Between t=0us and t=27.7us: Comparator positive input at level high, blanking source at level high.
   Blanking source is high, therefore comparator output is forced to level low.
   (otherwise, as comparator positive input is higher than negative input, comparator output would have been high.)
 - Between t=27.7us and t=duty cycle pulse: Comparator positive input at level high, blanking source at level low.
   Comparator positive input is higher than negative input, blanking source is low, therefore comparator output is high.
 - Between t=duty cycle pulse and t=period: Comparator positive input at level low, blanking source at level low.
   Comparator positive input is higher than negative input, therefore comparator output is low.

NUCLEO-H503RB board's LED is be used to monitor the program execution status:

  - LED2 is toggling every 500ms when test pass successfully.
  - LED2 is "ON" when an error occurred.

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides 
    accurate delay (in milliseconds) based on variable incremented in SysTick ISR. 
    This implies that if HAL_Delay() is called from a peripheral ISR process, then 
    the SysTick interrupt must have higher priority (numerically lower) than the 
    peripheral interrupt. Otherwise the caller ISR process will be blocked. 
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
      
 2. The application need to ensure that the SysTick time base is always set 
    to 1 millisecond to have correct HAL operation.

### <b>Keywords</b>

Comparator, COMP, analog, voltage, analog input, threshold, VrefInt, output blanking.

### <b>Directory contents</b>

  - COMP/COMP_OutputBlanking/Inc/stm32h5xx_nucleo_conf.h     BSP configuration file
  - COMP/COMP_OutputBlanking/Inc/stm32h5xx_hal_conf.h    HAL configuration file
  - COMP/COMP_OutputBlanking/Inc/stm32h5xx_it.h          COMP interrupt handlers header file
  - COMP/COMP_OutputBlanking/Inc/main.h                  Header for main.c module
  - COMP/COMP_OutputBlanking/Src/stm32h5xx_it.c          COMP interrupt handlers
  - COMP/COMP_OutputBlanking/Src/main.c                  Main program
  - COMP/COMP_OutputBlanking/Src/stm32h5xx_hal_msp.c     HAL MSP file 
  - COMP/COMP_OutputBlanking/Src/system_stm32h5xx.c      STM32H5xx system source file


### <b>Hardware and Software environment</b>

  - This example runs on STM32H503xx devices.

  - This example has been tested with NUCLEO-H503RB board and can be
    easily tailored to any other supported device and development board.

  - Connect comparator input as described in "Connection needed" section.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the example

