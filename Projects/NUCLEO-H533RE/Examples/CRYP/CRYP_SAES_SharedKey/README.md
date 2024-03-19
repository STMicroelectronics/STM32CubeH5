## <b>CRYP_SAES_SharedKey Example Description</b>

How to use the Secure AES co-processor (SAES) peripheral to share application keys
with AES peripheral.

At the beginning of the main program the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.

The SystemClock_Config() function is used to configure the system clock for STM32H533xx Devices :

  - The CPU at 250 MHz.

The Secure AES mode requires:
1. Application keys (AESKey256) to be encrypted and shared using DHUK(derived hardware unique key).
2. Plain text (Plaintext) which will be encrypted with AES peripheral.
3. Cipher text (CiphertextAESECB256)  which is the encryption result of the Plaint text.

In this example, LED2 (led GREEN) is turned on when the operation finish with success . 
and LED2 (led GREEN) is toggling on when the operation generate an error.

### <b>Notes</b>
1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
      based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
      a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
      than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
      To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

2. The application needs to ensure that the SysTick time base is always set to 1 millisecond
      to have correct HAL operation.

### <b>Keywords</b>

System, Security, SAES, AES, SharedKey

### <b>Directory contents</b>

  - CRYP/CRYP_SAES_SharedKey/Inc/stm32h5xx_nucleo_conf.h    BSP configuration file
  - CRYP/CRYP_SAES_SharedKey/Src/main.c                     Main program
  - CRYP/CRYP_SAES_SharedKey/Src/system_stm32h5xx.c         STM32H5xx system clock configuration file
  - CRYP/CRYP_SAES_SharedKey/Src/stm32h5xx_it.c             Interrupt handlers
  - CRYP/CRYP_SAES_SharedKey/Src/stm32h5xx_hal_msp.c        HAL MSP module
  - CRYP/CRYP_SAES_SharedKey/Inc/main.h                     Main program header file
  - CRYP/CRYP_SAES_SharedKey/Inc/stm32h5xx_hal_conf.h       HAL Configuration file
  - CRYP/CRYP_SAES_SharedKey/Inc/stm32h5xx_it.h             Interrupt handlers header file

### <b>Hardware and Software environment</b>

  - This example runs on STM32H533RETx devices.

  - This example has been tested with STMicroelectronics NUCLEO-H533RE
    board and can be easily tailored to any other supported device
    and development board.

### <b>How to use it ?</b> 

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example


