## <b>ECC_ECDSA_SignVerify_HAL Application Description</b>

This application describes how to use the PSA Ref API to sign and verify
a message using the ECDSA algorithm over SECP256 curve.

This application is configured to use the Hardware library.

This application demonstrates how to use the predefined curves parameters to perform
ECC operation in a single call method: the configuration of the algorithm, and the operation
is done in one single API call.

This application is scheduled as follow:

  - Generate digest of the known message
  - The signature of the known message digest using a known random, so that generated
    signature can be compared to the known signature
  - The verification of the know message digest with the known signature
  - The signature of the known message digest using a true random
  - The verification of the know message digest with this newly generated signature

####  <b>Expected success behavior</b>

- The green led will be turned on
- The global variable glob_status will be set to PASSED

#### <b>Error behaviors</b>

- The green led will be toggled each 250 milliseconds in an infinity loop.
- The global variable glob_status will be set to FAILED

### <b>Notes</b>
NIST vectors applications for ECDSA are taken from:
"ECDSA Test vectors / FIPS 186-4"
Available at:
 https://csrc.nist.gov/projects/cryptographic-algorithm-validation-program/digital-signatures
 Known_Signature is modified for matching k used by ARM (not the same as k of NIST)

### <b>Keywords</b>

Cryptography, authentication, ECDSA, Cryptographic

### <b>Directory contents</b>

  - MbedTLS_HW_ALT/ECC_ECDSA_SignVerify_HAL/Inc/stm32h5f5j_discovery_conf.h     BSP configuration file
  - MbedTLS_HW_ALT/ECC_ECDSA_SignVerify_HAL/Inc/stm32h5xx_hal_conf.h    HAL configuration file
  - MbedTLS_HW_ALT/ECC_ECDSA_SignVerify_HAL/Inc/stm32h5xx_it.h          Interrupt handlers header file
  - MbedTLS_HW_ALT/ECC_ECDSA_SignVerify_HAL/Inc/main.h                        Header for main.c module
  - MbedTLS_HW_ALT/ECC_ECDSA_SignVerify_HAL/Inc/mbedtls_config.h              Mbed TLS configuration file
  - MbedTLS_HW_ALT/ECC_ECDSA_SignVerify_HAL/Inc/mbedtls_alt_config.h          Mbed TLS Alt configuration file
  - MbedTLS_HW_ALT/ECC_ECDSA_SignVerify_HAL/Src/stm32h5xx_it.c          Interrupt handlers
  - MbedTLS_HW_ALT/ECC_ECDSA_SignVerify_HAL/Src/stm32h5xx_hal_msp.c     HAL MSP module
  - MbedTLS_HW_ALT/ECC_ECDSA_SignVerify_HAL/Src/main.c                        Main program
  - MbedTLS_HW_ALT/ECC_ECDSA_SignVerify_HAL/Src/system_stm32h5xx.c      STM32H5xx system source file

### <b>Hardware and Software environment</b>

  - This application runs on STM32H5xx devices.

  - This application has been tested with STM32H5F5J-DK board and can be
    easily tailored to any other supported device and development board.

###  <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the application
