## <b>TIM_OutputCompare_Init Example Description</b>

Configuration of the TIM peripheral to generate an output 
waveform in different output compare modes. This example is based on the 
STM32H5xx TIM LL API.
The peripheral initialization uses LL unitary service functions for optimization purposes (performance and size).

In this example TIM2 input clock (TIM2CLK) frequency is set to APB1 clock (PCLK1), since APB1 pre-scaler is equal to 1.

	TIM2CLK = PCLK1
	PCLK1 = HCLK
	=> TIM2CLK = HCLK = SystemCoreClock (250Mhz)

To set the TIM2 counter clock frequency to 10 KHz, the pre-scaler (PSC) is
calculated as follows:

	PSC = (TIM2CLK / TIM2 counter clock) - 1
	PSC = (SystemCoreClock /10 KHz) - 1

SystemCoreClock is set to 250 MHz for STM32H5xx Devices.

Auto-reload (ARR) is calculated to get a time base period of 100 ms,
meaning a time base frequency of 10 Hz.

	ARR = (TIM2 counter clock / time base frequency) - 1
	ARR = (TIM2 counter clock / 10) - 1

The capture/compare register (CCR1) of the output channel is set to half the
auto-reload value. Therefore the timer output compare delay is 50 ms.
Generally speaking this delay is calculated as follows:

	CC1_delay = TIM2 counter clock / CCR1

The timer output channel is mapped on the pin PA5 (connected to TIM2_CH1 on board
NUCLEO-H503RB).
Thus TIM2_CH1 status (on/off) mirrors the timer output level (active v.s. inactive).

USER push-button can be used to change the output compare mode:

  - When the output channel is configured in output compare toggle:  TIM2_CH1 
    TOGGLES when the counter (CNT) matches the capture/compare register (CCR1).
  - When the output channel is configured in output compare active:  TIM2_CH1 
    switched ON when the counter (CNT) matches the capture/compare register
    (CCR1).
  - When the output channel is configured in output compare inactive:  TIM2_CH1 
    switched OFF when the counter (CNT) matches the capture/compare register
    (CCR1).
    
Initially the output channel is configured in output compare toggle mode.

Note : that TIM2_CH1 share the same pin with LED2 PA5.

### <b>Keywords</b>

Timers, TIM, Output, signals, Output compare toggle, PWM, Oscilloscope

### <b>Directory contents</b>

  - TIM/TIM_OutputCompare_Init/Inc/stm32h5xx_it.h          Interrupt handlers header file
  - TIM/TIM_OutputCompare_Init/Inc/main.h                  Header for main.c module
  - TIM/TIM_OutputCompare_Init/Inc/stm32_assert.h          Template file to include assert_failed function
  - TIM/TIM_OutputCompare_Init/Src/stm32h5xx_it.c          Interrupt handlers
  - TIM/TIM_OutputCompare_Init/Src/main.c                  Main program
  - TIM/TIM_OutputCompare_Init/Src/system_stm32h5xx.c      STM32H5xx system source file

### <b>Hardware and Software environment</b>

  - This example runs on STM32H503RBTx devices.

  - This example has been tested with NUCLEO-H503RB board and can be
    easily tailored to any other supported device and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

