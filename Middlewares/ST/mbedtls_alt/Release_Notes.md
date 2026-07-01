---
pagetitle: Release Notes for STM32 Middleware Mbed TLS Alt
lang: en
header-includes: <link rel="icon" type="image/x-icon" href="_htmresc/favicon.png" />
---

::: {.row}
::: {.col-sm-12 .col-lg-6}

<center>
# Release Notes for STM32 Mbed TLS Alt
Copyright &copy; 2024 STMicroelectronics\
</center>

[![ST logo](_htmresc/st_logo_2020.png)](https://www.st.com)

## __Summary__
This component is an alternative cryptography implementation of Mbed TLS cryptography modules.

STM32 Mbed TLS Alt provides a set of alternative functions to replace Mbed TLS cryptography modules
by alternative implementation based on STM32 hardware cryptographic accelerators.

## __Description__
* **This component repository contains two types of alternative cryptography implementation:**

  * _"HAL"_ means alternative cryptography implementation based on STM32 hardware cryptographic accelerator.
  * _"ITS"_ means alternative internal encrypted storage implementation to replace PSA ITS over files module.

 **_NOTE:_**
    ITS alternative implementation could be encrypted using user key imported with a specific unique ID or
    the hardware unique key (HUK) when the IP SAES is supported and the flag USE_HUK is enabled.
    ITS alternative implementation needs a storage interface depending to user application.
    `Encrypted_ITS_KeyImport` example is provided to demonstrate how to store AES-CBC key using encrypted ITS.

* **Severals preprocessor symbols are created to distinguish cryptography modules:**

  * `MBEDTLS_HAL_AES_ALT` : to use STM32 hardware AES cryptographic accelerator for AES implementation.
  * `MBEDTLS_HAL_GCM_ALT` : to use STM32 hardware AES cryptographic accelerator for GCM implementation.
  * `MBEDTLS_HAL_CCM_ALT` : to use STM32 hardware AES cryptographic accelerator for CCM implementation.
  * `MBEDTLS_HAL_SHA256_ALT` : to use STM32 hardware HASH cryptographic accelerator for SHA256 implementation.
  * `MBEDTLS_HAL_SHA1_ALT` : to use STM32 hardware HASH cryptographic accelerator for SHA1 implementation.
  * `MBEDTLS_HAL_ECDSA_ALT` : to use STM32 hardware PKA cryptographic accelerator for ECDSA implementation.
  * `MBEDTLS_HAL_ECDH_ALT` : to use STM32 hardware PKA cryptographic accelerator for ECDH implementation.
  * `MBEDTLS_HAL_RSA_ALT` : to use STM32 hardware PKA cryptographic accelerator for RSA implementation.
  * `MBEDTLS_HAL_ENTROPY_HARDWARE_ALT` : to use STM32 hardware RNG cryptographic accelerator for entropy source implementation.

* **Two preprocessor symbols are created to manage ITS alternative implementation:**
  * `PSA_USE_ITS_ALT` : to enable ITS alternative module to replace PSA ITS over files module by ITS alternative implementation.
  * `PSA_USE_ENCRYPTED_ITS` : to enable ITS encryption feature for ITS alternative module using imported user key or HUK.

 **_NOTE:_**
    AES supports protection against side-channel attacks using DPA preprocessor symbols:

  * `HW_CRYPTO_DPA_AES` : to use STM32 hardware SAES cryptographic accelerator for DPA AES implementation.
  * `HW_CRYPTO_DPA_GCM` : to use STM32 hardware SAES cryptographic accelerator for DPA AES GCM implementation.

  There is a couple of methods for alternative implementations: specific function replacement and full module replacement.

  This version of middleware supports specific function replacement and module replacement, for example
  `MBEDTLS_HAL_SHA256_ALT` may be defined to replace the full Mbed TLS SHA256 module with a hardware accelerated SAH256 module.

* **Supported alternative cryptography implementation**

<center>

| Algorithms           | Mbed TLS  | STM32 Mbed TLS Alt  |
| -------------------- |:---------:| -------------------:|
| AES (ECB, CTR, CBC)  |    YES    |        YES          |
| AES (GCM, CCM)       |    YES    |        YES          |
| ChachaPoly           |    YES    |        NO           |
| ECDH                 |    YES    |        YES          |
| ECDSA                |    YES    |        YES          |
| HASH (SHA2, SHA1)    |    YES    |        YES          |
| CMAC                 |    YES    |        NO           |
| HMAC                 |    YES    |        YES          |
| RSA PKCS(v1.5, v2.2) |    YES    |        YES          |

</center>

## __How to use it?__

 This can be achieved by :

  * Adding STM32 Mbed TLS Alt middleware to user application.

  * Adding Mbed TLS middleware to user application.

  * Defining the appropriate `MBEDTLS_*_ALT` preprocessor symbol in `mbedtls_config.h` for each module that needs to be replaced.

  * Defining the appropriate preprocessor `MBEDTLS_HAL_*_ALT` in mbedtls_alt_config.h to use STM32 hardware cryptographic accelerator alternative.

  **Use case : replace mbed TLS AES module by STM32 hardware accelerated AES module**

  * Add STM32 Mbed TLS Alt middleware to user application and uncomment the macro `MBEDTLS_HAL_AES_ALT` in `mbedtls_alt_config.h` file.

  * Add Mbed TLS library to user application and uncomment the macro `MBEDTLS_AES_ALT` in `mbedtls_config.h` to let mbed TLS use the STM32 hardware accelerated AES module.

  * Add STM32Cube HAL drivers for STM32 hardware cryptographic accelerator to user application as below:
    - Add RNG HAL driver : `stm32XXxx_hal_rng.c` and `stm32XXxx_hal_rng_ex.c`
    - Add AES HAL driver : `stm32XXxx_hal_cryp.c` and `stm32XXxx_hal_cryp_ex.c`

## __Keywords__

   AES, GCM, CBC, Chacha20-Poly1305, RNG, DRBG, ECC, ECDSA, ECDH, HASH, SHA2, CMAC, HMAC, RSA, PKCS#1 v1.5, PKCS#1 v2.2,
   ENCRYPT, DECRYPT, SIGN, VERIFY, Mbed TLS, PSA Crypto API, STM32 Mbed TLS Alt, Encrypted ITS.
:::

::: {.col-sm-12 .col-lg-6}
# Update History

\

::: {.collapse}
<input type="checkbox" id="collapse-section5" checked aria-hidden="true">
<label for="collapse-section5" aria-hidden="true">__V1.2.1 / 03-June-2026__</label>
<div>

## Main changes

-  Add SW Security Classification file, SW_Security_Level.md


## Known limitations
- None

## Backward compatibility

- None.

## Dependencies

- This component depends on:
  - Mbed TLS cryptography library v3.6.6

</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section4" aria-hidden="true">
<label for="collapse-section4" aria-hidden="true">__V1.2.0 / 31-March-2026__</label>
<div>

## Main changes

-  Support MBEDTLS_HAL_CCM_MULTIPART_ALT for CCM finish and update
-  CCM: Handle output buffer for multipart with buffering data

## Known limitations
- None

## Backward compatibility

- None.

## Dependencies

- This component depends on:
  - Mbed TLS cryptography library v3.6.6

</div>
:::


::: {.collapse}
<input type="checkbox" id="collapse-section3" aria-hidden="true">
<label for="collapse-section3" aria-hidden="true">__V1.1.1 / 20-May-2025__</label>
<div>

## Main changes

- Set prime order size in ecdh compute shared restartable
- Replace error handler in entropy hardware alternative with a return status
- Fix the output length in the GCM alternative
- Update the AES CTR alternative to rely on a full hardware implementation
- Increase ITS object size

## Known limitations
- None

## Backward compatibility

- None.

## Dependencies

- This component depends on:
  - Mbed TLS cryptography library version v3.6.2 or higher

</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section2" aria-hidden="true">
<label for="collapse-section2" aria-hidden="true">__V1.1.0 / 23-January-2025__</label>
<div>

## Main changes

- Add CCM alternative cryptography implementation
- Add SHA1 alternative cryptography implementation
- Add ITS encryption using the hardware unique key (HUK)
- Path update to interfaces/patterns

## Known limitations
- None

## Backward compatibility

- None.

## Dependencies

- This component depends on:
  - Mbed TLS cryptography library v3.6.2

</div>
:::


::: {.collapse}
<input type="checkbox" id="collapse-section1" aria-hidden="true">
<label for="collapse-section1" aria-hidden="true">__V1.0.0 / 29-October-2024__</label>
<div>

## Main changes

- First release

## Known limitations
- None

## Backward compatibility

- None.

## Dependencies

- This component depends on:
  - Mbed TLS cryptography library v3.6.1

</div>
:::


:::
:::

<footer class="sticky">
::: {.columns}
::: {.column width="95%"}
For complete documentation on **STM32 Microcontrollers** ,
visit: [www.st.com/STM32](http://www.st.com/STM32)
:::
::: {.column width="5%"}
<abbr title="Based on template cx566953 version 3.0">Info</abbr>
:::
:::
</footer>