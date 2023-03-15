## <b>SAI_AudioPlay Example Description</b>

This example shows how to use the SAI HAL API to play an audio file using the DMA
circular mode and how to handle the buffer update.

      Plug a headphone to ear the sound  /!\ Take care of yours ears.
      Default volume is 80%.
      The audio file is played in loop
      **Note :** Copy file 'audio.bin' (available in AudioFile) directly in the flash
      at @0x08080000 using ST-Link utility or STM32CubeProgrammer

Once started, LED1 is toggling on STM32H573I-DK.

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate
    delay (in milliseconds) based on variable incremented in SysTick ISR. This
    implies that if HAL_Delay() is called from a peripheral ISR process, then
    the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The application need to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Keywords</b>

Audio, SAI, Audio playback, Headphone, Audio codec, DMA

### <b>Directory contents</b>

  - SAI/SAI_AudioPlay/Inc/stm32h573i_discovery_conf.h BSP configuration file
  - SAI/SAI_AudioPlay/Src/main.c                      Main program
  - SAI/SAI_AudioPlay/Src/system_stm32h5xx.c          STM32H5xx system source file
  - SAI/SAI_AudioPlay/Src/stm32h5xx_it.c              Interrupt handlers
  - SAI/SAI_AudioPlay/Inc/main.h                      Main program header file
  - SAI/SAI_AudioPlay/Inc/stm32h5xx_hal_conf.h        HAL configuration file
  - SAI/SAI_AudioPlay/Inc/stm32h5xx_it.h              Interrupt handlers header file
  - SAI/SAI_AudioPlay/AudioFile/audio.bin             Audio wave extract.


### <b>Hardware and Software environment</b>

  - This example runs on STM32H573IIKxQ devices.

  - This example has been tested with STMicroelectronics STM32H573I-DK
    board and can be easily tailored to any other supported device
    and development board.

  - STM32H573I-DK Set-up :
    - Plug headset on CN9 connector (Headset).

### <b>How to use it ?</b>

In order to make the program work, you must do the following:

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example


