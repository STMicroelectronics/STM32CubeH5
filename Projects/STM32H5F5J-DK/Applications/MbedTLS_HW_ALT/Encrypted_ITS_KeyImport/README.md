## <b>Encrypted_ITS_KeyImport Application Description</b>

This application describes how to use the PSA ITS alternative encrypted implementation to import AES-CBC
key and store it in user persistent storage, the key is encrypted by the ITS.

This application is configured to use the Hardware library.

This application demonstrates how to use the unique identifier ITS_ENCRYPTION_SECRET_KEY_ID to perform
persistent key storein encrypted storage: the configuration of the algorithm, and the operation
is done in one single API call.

This application is scheduled as follow:

  - Initialize the PSA Crypto Library.
  - Import ITS secret key (AES GCM ) using predefined ITS_ENCRYPTION_SECRET_KEY_ID unique identifier.
  - Import user AES-CBC key to be stored in user persistent storage.
  - Close the user AES-CBC key to remove it from volatile memory.
  - Open the user AES-CBC key to load it from persistent encrypted storage.
  - Use the user AES-CBC key to encrypt a message
  - Use the user AES-CBC  key to decrypt the previously computed ciphertext so
    that generated plaintext can be compared with the known message.
  - Destroy the keys to free up resources.

####  <b>Expected success behavior</b>

- The green led will be turned on
- The global variable glob_status will be set to PASSED

#### <b>Error behaviors</b>

- The green led will be toggled each 250 milliseconds in an infinity loop.
- The global variable glob_status will be set to FAILED

### <b>Notes</b>
NIST vectors applications for AES CBC are taken from:
"Recommendation for Block Cipher Modes of Operation, 2001 Edition"
Available at:
 https://csrc.nist.gov/publications/detail/sp/800-38a/final

### <b>Keywords</b>

Cryptography, AES, GCM, CBC, Encrypted ITS, Cryptographic

### <b>Directory contents</b>

  - MbedTLS_HW_ALT/Encrypted_ITS_KeyImport/Inc/stm32h5f5j_discovery_conf.h     BSP configuration file

  - MbedTLS_HW_ALT/Encrypted_ITS_KeyImport/Inc/stm32h5xx_hal_conf.h    HAL configuration file
  - MbedTLS_HW_ALT/Encrypted_ITS_KeyImport/Inc/stm32h5xx_it.h          Interrupt handlers header file
  - MbedTLS_HW_ALT/Encrypted_ITS_KeyImport/Inc/main.h                        Header for main.c module
  - MbedTLS_HW_ALT/Encrypted_ITS_KeyImport/Inc/mbedtls_config.h              Mbed TLS configuration file
  - MbedTLS_HW_ALT/Encrypted_ITS_KeyImport/Inc/mbedtls_alt_config.h          Mbed TLS Alt configuration file
  - MbedTLS_HW_ALT/Encrypted_ITS_KeyImport/Inc/storage_interface.h           Storage header file
  - MbedTLS_HW_ALT/Encrypted_ITS_KeyImport/Src/stm32h5xx_it.c          Interrupt handlers
  - MbedTLS_HW_ALT/Encrypted_ITS_KeyImport/Src/stm32h5xx_hal_msp.c     HAL MSP module
  - MbedTLS_HW_ALT/Encrypted_ITS_KeyImport/Src/main.c                        Main program
  - MbedTLS_HW_ALT/Encrypted_ITS_KeyImport/Src/storage_interface.c           Storage source file
  - MbedTLS_HW_ALT/Encrypted_ITS_KeyImport/Src/system_stm32h5xx.c      STM32H5xx system source file

### <b>Hardware and Software environment</b>

  - This application runs on STM32H5xx devices.

  - This application has been tested with STM32H5F5J-DK board and can be
    easily tailored to any other supported device and development board.

###  <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the application
