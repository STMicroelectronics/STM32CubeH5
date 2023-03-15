## <b>CRYP_AES_CCM Example Description</b>

How to use the CRYPTO peripheral to encrypt and decrypt data using AES with
cipher block chaining-message authentication code(CCM).

At the beginning of the main program the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.

The SystemClock_Config() function is used to configure the system clock for STM32H573IIKxQ Devices :

  - The CPU at 250 MHz.

### The AES-CCM mode requires:  

1. Plain text: which will be authenticated and encrypted. Its size must be a multiple
   of 16 bytes. So if the original plain text size is not a multiple of 16 bytes, it must be padded.    
2. Header (associated data): that will be authenticated but not encrypted.  
   The header if formatted in HeaderAES: the header size is concatenated with the original header.  
3. B0: first authentication block used only  in this mode.  
4. Key: is the parameter which determines the cipher text. In this example the key size is  128 bits.  

### The AES-CCM provides:  
1. Cipher text: which is the encryption result of Plain text. In this example, the cipher text is available in Ciphertext[].  
   Its size is the same as the plain text.  
2. TAG: the authentication TAG which is used for both message authentication  
   and message tampering detection. In this example, the size is equal to 16 bytes.  


The green LED (LED1) is turned ON, in case the encryption/decryption and tag are  
computed correctly, in case there is a mismatch the red LED is turned ON (LED3).  

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The application needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Keywords</b>

Security, CRYP, AES, TrustZone disabled

### <b>Directory contents</b>

  - CRYP/CRYP_AES_CCM/Inc/stm32h573i_discovery_conf.h     BSP Configuration file
  - CRYP/CRYP_AES_CCM/Inc/stm32h5xx_hal_conf.h    			HAL configuration file
  - CRYP/CRYP_AES_CCM/Inc/stm32h5xx_it.h          			Interrupt handlers header file
  - CRYP/CRYP_AES_CCM/Inc/main.h                  			Header for main.c module
  - CRYP/CRYP_AES_CCM/Src/stm32h5xx_it.c          			Interrupt handlers
  - CRYP/CRYP_AES_CCM/Src/main.c                  			Main program
  - CRYP/CRYP_AES_CCM/Src/stm32h5xx_hal_msp.c     			HAL MSP module
  - CRYP/CRYP_AES_CCM/Src/system_stm32h5xx.c      			STM32H5xx system source file

### <b>Hardware and Software environment</b>

  - This example runs on STM32H573IIKxQ devices.

  - This template has been tested with STMicroelectronics STM32H573I-DK
    board and can be easily tailored to any other supported device
    and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following:

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example


