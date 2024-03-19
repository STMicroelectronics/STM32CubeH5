## <b>PKA_ECDSA_Verify Example Description</b>

How to determine if a given signature is valid regarding the Elliptic curve digital signature algorithm
(ECDSA).

For this example, a test vector have been extracted from National Institute of Standards and Technology (NIST)

 - Cryptographic Algorithm Validation Program (CAVP) in order to demonstrate the usage of the hal.
This reference files can be found under:

"http://csrc.nist.gov/groups/STM/cavp/documents/dss/186-3ecdsatestvectors.zip (ZIP SigGen.rsp)"

This test vector has been chosen to demonstrate the behavior in a case where the input signature
is valid. A second input is provided where one element of the hash message has been modified to 
demonstrate the behavior in a case where the signature is invalid. Their definitions are included 
in SigVer.c. You can refer to this file for more information.

The selected curve for this example is P-256 (ECDSA-256) published by NIST in 
Federal Information Processing Standards Publication FIPS PUB 186-4. The description
of this curve is present in file Src/prime256v1.c.

In case of success, the LD1 (Yellow) is ON.

In case of any error, the LD3 (Red) is toggling slowly.

### <b>Keywords</b>

Security, PKA, NIST, CAVP, ECDSA verification

### <b>Directory contents</b>

  - PKA/PKA_ECDSA_Verify/Inc/stm32wbaxx_hal_conf.h    HAL configuration file
  - PKA/PKA_ECDSA_Verify/Inc/stm32wbaxx_it.h          Interrupt handlers header file
  - PKA/PKA_ECDSA_Verify/Inc/main.h                   Header for main.c module
  - PKA/PKA_ECDSA_Verify/Src/stm32wbaxx_it.c          Interrupt handlers
  - PKA/PKA_ECDSA_Verify/Src/main.c                   Main program
  - PKA/PKA_ECDSA_Verify/Src/stm32wbaxx_hal_msp.c     HAL MSP module 
  - PKA/PKA_ECDSA_Verify/Src/system_stm32wbaxx.c      STM32WBAxx system source file
  - PKA/PKA_ECDSA_Verify/Src/prime256v1.c             Description of P-256 (ECDSA-256)
  - PKA/PKA_ECDSA_Verify/Inc/prime256v1.h             Header for prime256v1.c
  - PKA/PKA_ECDSA_Verify/Src/SigVer.c                 Reflect the content of the test vector from SigVer.rsp
  - PKA/PKA_ECDSA_Verify/Src/SigVer.rsp               Extract from NIST CAVP
  - PKA/PKA_ECDSA_Verify/Inc/SigVer.h                 Header of SigVer.c

### <b>Hardware and Software environment</b>

  - This example runs on STM32H563ZITx devices.

  - This example has been tested with NUCLEO-H563ZI board and can be
    easily tailored to any other supported device and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following:

 - Open your preferred toolchain 
 - Rebuild all files and load your image into target memory
 - Run the example