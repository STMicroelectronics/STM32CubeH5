## <b>Cipher_AES_GCM_Encrypt_Decrypt_KWE Application Description</b>

This application describes how to use the PSA Crypto opaque driver based on STM32
Key Wrape Engine to wrap the AES-GCM private key and use the wrapped key to
perform authenticated encryption and verified the decryption using the
AES GCM algorithm.

This application is configured to use the Hardware library.

This application demonstrates how to use the PSA crypto API to perform
AES-GCM operation in a single call method: the configuration of the algorithm,
and the operation is done in one single API call.

This application is scheduled as follow:

  - Initialize the PSA Crypto Library.
  - Wrap the private key using key import operation and KWE driver location.
  - Use the wrapped private key to encrypt the known message.
  - Verify generated data are the expected ones.
  - Verify generated authentication tag is the expected one.
  - Use the wrapped private key to decrypt the previously computed ciphertext so
    that generated plaintext can be compared with the known message.
  - Destroy the key to free up resources.

####  <b>Expected success behavior</b>

- The green led will be turned on
- The global variable glob_status will be set to PASSED

#### <b>Error behaviors</b>

- The green led will be toggled each 250 milliseconds in an infinity loop.
- The global variable glob_status will be set to FAILED

### <b>Notes</b>
NIST vectors applications for AES GCM are taken from:
"Cryptographic Standards and Guidelines / applications with Intermediate Values"
Available at:
 https://csrc.nist.gov/projects/cryptographic-standards-and-guidelines/application-values

### <b>Keywords</b>

Cryptography, authentication, AES, GCM, Wrap, Cryptographic

### <b>Directory contents</b>

  - MbedTLS_HW_KWE/Cipher_AES_GCM_Encrypt_Decrypt_KWE/Inc/stm32h5f5j_discovery_conf.h     BSP configuration file

  - MbedTLS_HW_KWE/Cipher_AES_GCM_Encrypt_Decrypt_KWE/Inc/stm32h5xx_hal_conf.h    HAL configuration file
  - MbedTLS_HW_KWE/Cipher_AES_GCM_Encrypt_Decrypt_KWE/Inc/stm32h5xx_it.h          Interrupt handlers header file
  - MbedTLS_HW_KWE/Cipher_AES_GCM_Encrypt_Decrypt_KWE/Inc/main.h                        Header for main.c module
  - MbedTLS_HW_KWE/Cipher_AES_GCM_Encrypt_Decrypt_KWE/Inc/storage_interface.h           Storage header file
  - MbedTLS_HW_KWE/Cipher_AES_GCM_Encrypt_Decrypt_KWE/Inc/mbedtls_config.h              Mbed TLS configuration file
  - MbedTLS_HW_KWE/Cipher_AES_GCM_Encrypt_Decrypt_KWE/Inc/mbedtls_alt_config.h          Mbed TLS Alt configuration file
  - MbedTLS_HW_KWE/Cipher_AES_GCM_Encrypt_Decrypt_KWE/Inc/kwe_config.h                  Key Wrap Engine configuration file
  - MbedTLS_HW_KWE/Cipher_AES_GCM_Encrypt_Decrypt_KWE/Src/stm32h5xx_it.c          Interrupt handlers
  - MbedTLS_HW_KWE/Cipher_AES_GCM_Encrypt_Decrypt_KWE/Src/stm32h5xx_hal_msp.c     HAL MSP module
  - MbedTLS_HW_KWE/Cipher_AES_GCM_Encrypt_Decrypt_KWE/Src/main.c                        Main program
  - MbedTLS_HW_KWE/Cipher_AES_GCM_Encrypt_Decrypt_KWE/Src/storage_interface.c           Storage source file
  - MbedTLS_HW_KWE/Cipher_AES_GCM_Encrypt_Decrypt_KWE/Src/system_stm32h5xx.c      STM32H5xx system source file

### <b>Hardware and Software environment</b>

  - This application runs on STM32H5xx devices.

  - This application has been tested with STM32H5F5J-DK board and can be
    easily tailored to any other supported device and development board.

###  <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the application
