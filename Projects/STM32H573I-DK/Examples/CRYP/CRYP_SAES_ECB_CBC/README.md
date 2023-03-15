## <b>CRYP_SAES_ECB_CBC Example Description</b>

How to use the Secure AES co-processor (SAES) peripheral to encrypt and decrypt data
using AES ECB and CBC algorithms.

This test is from cryptographic algorithm validation program in National Institute of Standards and Technology (NIST)
https://github.com/coruus/nist-testvectors/blob/master/csrc.nist.gov/groups/ST/toolkit/documents/Examples/AES_ECB.txt
https://csrc.nist.gov/CSRC/media/Projects/Cryptographic-Standards-and-Guidelines/documents/examples/AES_CBC.pdf

At the beginning of the main program the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.
The SystemClock_Config() function is used to configure the system clock for STM32H573IIKxQ Devices :
The CPU at 250 MHz.

The Secure AES mode requires:

1. Plain text which will be encrypted. Its size must be a multiple of 16 bytes.
   So if the original plain text size is not a multiple of 16 bytes, it must be padded.
2. Cipher text which is the encryption result of the Plaint text. In this
   example, the cipher text is available in "EncryptedText".
3. Initialization vector used only for CBC Algorithm.
4. In this example the key size is 128 bits =16 bytes for ECB and 256 bits for CBC.

After startup, the Secure AES is configured to encrypt/decrypt using ECB algorithm
and 128-bit key length, then change the configuration to use CBC algorithm and
256-bit key length then perform the encrypt/decrypt.
All encrypted texts are compared to known results in CiphertextAESECB128 and
CiphertextAESCBC256 buffers.
Finally, LED1 (led green)) is toggling if all encrypted/decrypted are as expected and
LED3 (led red) is turned on when the operation generates an error or when the ciphering
or the deciphering output doesn't match the expected result.


#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The application needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.


### <b>Keywords</b>

System, Security, SAES, AES, CRYP, ECB, CBC

### <b>Directory contents</b>

  - CRYP/CRYP_SAES_ECB_CBC/Inc/stm32h573i_discovery_conf.h  BSP configuration file
  - CRYP/CRYP_SAES_ECB_CBC/Inc/stm32h5xx_hal_conf.h    		HAL configuration file
  - CRYP/CRYP_SAES_ECB_CBC/Inc/stm32h5xx_it.h          		Interrupt handlers header file
  - CRYP/CRYP_SAES_ECB_CBC/Inc/main.h                  		Header for main.c module
  - CRYP/CRYP_SAES_ECB_CBC/Src/stm32h5xx_it.c          		Interrupt handlers
  - CRYP/CRYP_SAES_ECB_CBC/Src/main.c                  		Main program
  - CRYP/CRYP_SAES_ECB_CBC/Src/stm32h5xx_hal_msp.c     		HAL MSP module
  - CRYP/CRYP_SAES_ECB_CBC/Src/system_stm32h5xx.c      		STM32H5xx system source file

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


