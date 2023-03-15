## <b>TIM_OCToggle Example Description</b>

Configuration of the TIM peripheral to generate four different 
signals at four different frequencies.

At the beginning of the main program the HAL_Init() function is called to reset 
all the peripherals, initialize the Flash interface and the systick.
The SystemClock_Config() function is used to configure the system clock for STM32H503RBTx Devices :

The CPU at 250 MHz

The TIM1 frequency is set to SystemCoreClock, and the objective is
to get TIM1 counter clock at 1 MHz so the Prescaler is computed as following:

     - Prescaler = (TIM1CLK /TIM1 counter clock) - 1
   
- SystemCoreClock is set to 250 MHz for STM32H5xx Devices.

- The TIM1 CCR1 register value is equal to 625:
  CC1 update rate = TIM1 counter clock / CCR1_Val = 1600 Hz,
  so the TIM1 Channel 1 generates a periodic signal with a frequency equal to 800 Hz.

- The TIM1 CCR2 register value is equal to 1250:
  CC2 update rate = TIM1 counter clock / CCR2_Val = 800 Hz,
  so the TIM1 channel 2 generates a periodic signal with a frequency equal to 400 Hz.

- The TIM1 CCR3 register value is equal to 2500:
  CC3 update rate = TIM1 counter clock / CCR3_Val = 400 Hz,
  so the TIM1 channel 3 generates a periodic signal with a frequency equal to 200 Hz.

- The TIM1 CCR4 register value is equal to 5000:
  CC4 update rate = TIM1 counter clock / CCR4_Val =  200 Hz,
  so the TIM1 channel 4 generates a periodic signal with a frequency equal to 100 Hz.

#### <b>Notes</b>

 1. PWM signal frequency values mentioned above are theoretical (obtained when the system clock frequency
    is exactly 250 MHz). Since the generated system clock frequency may vary from one board to another observed
    PWM signal frequency might be slightly different.

 2. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 3. This example needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Keywords</b>

Timer, TIM, Output, signals, Output compare toggle, PWM, Oscilloscope

### <b>Directory contents</b>

  - TIM/TIM_OCToggle/Inc/stm32h5xx_nucleo_conf.h BSP configuration file
  - TIM/TIM_OCToggle/Inc/stm32h5xx_hal_conf.h    HAL configuration file
  - TIM/TIM_OCToggle/Inc/stm32h5xx_it.h          Interrupt handlers header file
  - TIM/TIM_OCToggle/Inc/main.h                  Header for main.c module
  - TIM/TIM_OCToggle/Src/stm32h5xx_it.c          Interrupt handlers
  - TIM/TIM_OCToggle/Src/main.c                  Main program
  - TIM/TIM_OCToggle/Src/stm32h5xx_hal_msp.c     HAL MSP file
  - TIM/TIM_OCToggle/Src/system_stm32h5xx.c      STM32H5xx system source file

### <b>Hardware and Software environment</b>

  - This example runs on STM32H503RBTx devices.
    
  - This example has been tested with STMicroelectronics NUCLEO-H503RB 
    board and can be easily tailored to any other supported device 
    and development board.      

  - NUCLEO-H503RB Set-up
   Connect the following pins to an oscilloscope to monitor the different waveforms:
      - PB1: (TIM1_CH1) (pin 24 in CN10 connector)
      - PC7: (TIM1_CH2) (pin 1 in CN5 connector)
	  - PA1: (TIM1_CH3) (pin 2 in CN8 connector)
	  - PA2: (TIM1_CH4) (pin 3 in CN8 connector)

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example

