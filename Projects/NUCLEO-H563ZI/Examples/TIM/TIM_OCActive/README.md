## <b>TIM_OCActive Example Description</b> 

Configuration of the TIM peripheral in Output Compare Active mode 
(when the counter matches the capture/compare register, the corresponding output 
pin is set to its active state).

  The TIM1 frequency is set to SystemCoreClock, and the objective is
  to get TIM1 counter clock at 10 kHz so the Prescaler is computed as following:

     - Prescaler = (TIM1CLK /TIM1 counter clock) - 1
   
  SystemCoreClock is set to 250 MHz for STM32H5xx Devices.

  The TIM1 CCR1 register value is equal to 10000:

     TIM1_CH1 delay = CCR1_Val/TIM1 counter clock  = 1s
     so the TIM1 Channel 1 generates a signal with a delay equal to 1s.

  The TIM1 CCR2 register value is equal to 5000:

     TIM1_CH2 delay = CCR2_Val/TIM1 counter clock = 500 ms
     so the TIM1 Channel 2 generates a signal with a delay equal to 500 ms.

  The TIM1 CCR3 register value is equal to 2500:

     TIM1_CH3 delay = CCR3_Val/TIM1 counter clock = 250 ms
     so the TIM1 Channel 3 generates a signal with a delay equal to 250 ms.

  The TIM1 CCR4 register value is equal to 1250:

     TIM1_CH4 delay = CCR4_Val/TIM1 counter clock = 125 ms
     so the TIM1 Channel 4 generates a signal with a delay equal to 125 ms.

  The delay correspond to the time difference between PB0 rising edge and
  TIM1_CHx signal rising edges.

  STM32 board LED can be used to monitor the example status:

  - LED1 turns ON if example is OK.
  - LED3 toggles slowly in case of error.

#### <b>Notes</b>

 1. Delay values mentioned above are theoretical (obtained when the system clock frequency 
    is exactly 250 MHz). Since the generated system clock frequency may vary from one board to another observed
    delay might be slightly different.

 2. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
      
 3. This example needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Keywords</b>

Timer, TIM, Output, Compare, Active, Signals, 

### <b>Directory contents</b>

  - TIM/TIM_OCActive/Inc/stm32h5xx_nucleo_conf.h BSP configuration file
  - TIM/TIM_OCActive/Inc/stm32h5xx_hal_conf.h    HAL configuration file
  - TIM/TIM_OCActive/Inc/stm32h5xx_it.h          Interrupt handlers header file
  - TIM/TIM_OCActive/Inc/main.h                  Header for main.c module  
  - TIM/TIM_OCActive/Src/stm32h5xx_it.c          Interrupt handlers
  - TIM/TIM_OCActive/Src/main.c                  Main program
  - TIM/TIM_OCActive/Src/stm32h5xx_hal_msp.c     HAL MSP file
  - TIM/TIM_OCActive/Src/system_stm32h5xx.c      STM32H5xx system source file

### <b>Hardware and Software environment</b>

  - This example runs on STM32H563ZITx devices.

  - This example has been tested with STMicroelectronics NUCLEO-H563ZI 
    board and can be easily tailored to any other supported device 
    and development board.

  - NUCLEO-H563ZI Set-up
   Connect the following pins to an oscilloscope to monitor the different waveforms:
      - Use LED1 connected to PB0 (Reference) (pin 31 in CN8 connector)
      - PE9: (TIM1_CH1) (pin 4 in CN8 connector)
      - PE11: (TIM1_CH2) (pin 6 in CN8 connector)
      - PE13: (TIM1_CH3) (pin 10 in CN8 connector)
      - PE14: (TIM1_CH4) (pin 8 in CN8 connector)

### <b>How to use it ?</b> 

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example


