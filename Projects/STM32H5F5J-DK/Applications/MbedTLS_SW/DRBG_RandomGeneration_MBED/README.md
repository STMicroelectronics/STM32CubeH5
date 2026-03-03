## <b>DRBG_RandomGeneration_MBED Application Description</b>

This application describes how to use the PSA Ref API to generate random numbers
using the DRBG module.

This application is configured to use the MBED library.

This application is scheduled as follow:

  - The generation of random numbers.
    So that produced random ease different at each run.

####  <b>Expected success behavior</b>

- The green led will be turned on
- The global variable glob_status will be set to PASSED

#### <b>Error behaviors</b>

- The green led will be toggled each 250 milliseconds in an infinity loop.
- The global variable glob_status will be set to FAILED

### <b>Keywords</b>

Cryptography, random, DRBG, Cryptographic

### <b>Directory contents</b>

  - MbedTLS_SW/DRBG_RandomGeneration_MBED/Inc/stm32h5f5j_discovery_conf.h     BSP configuration file
  - MbedTLS_SW/DRBG_RandomGeneration_MBED/Inc/stm32h5xx_hal_conf.h    HAL configuration file
  - MbedTLS_SW/DRBG_RandomGeneration_MBED/Inc/stm32h5xx_it.h          Interrupt handlers header file
  - MbedTLS_SW/DRBG_RandomGeneration_MBED/Inc/main.h                        Header for main.c module
  - MbedTLS_SW/DRBG_RandomGeneration_MBED/Inc/mbedtls_config.h              Mbed TLS configuration file
  - MbedTLS_SW/DRBG_RandomGeneration_MBED/Inc/mbedtls_alt_config.h          Mbed TLS Alt configuration file
  - MbedTLS_SW/DRBG_RandomGeneration_MBED/Src/stm32h5xx_it.c          Interrupt handlers
  - MbedTLS_SW/DRBG_RandomGeneration_MBED/Src/stm32h5xx_hal_msp.c     HAL MSP module
  - MbedTLS_SW/DRBG_RandomGeneration_MBED/Src/main.c                        Main program
  - MbedTLS_SW/DRBG_RandomGeneration_MBED/Src/system_stm32h5xx.c      STM32H5xx system source file

### <b>Hardware and Software environment</b>

  - This application runs on STM32H5xx devices.

  - This application has been tested with STM32H5F5J-DK board and can be
    easily tailored to any other supported device and development board.

###  <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the application

