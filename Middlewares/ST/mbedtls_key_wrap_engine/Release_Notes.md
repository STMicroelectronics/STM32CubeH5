---
pagetitle: Release Notes for STM32 Middleware Mbed TLS Key Wrap Engine
lang: en
header-includes: <link rel="icon" type="image/x-icon" href="_htmresc/favicon.png" />
---

::: {.row}
::: {.col-sm-12 .col-lg-6}

<center>
# Release Notes for STM32 Mbed TLS Key Wrap Engine
Copyright &copy; 2024 STMicroelectronics\
</center>

[![ST logo](_htmresc/st_logo_2020.png)](https://www.st.com)

## __Summary__
The STM32 Mbed TLS Key Wrap Engine is a software component that provides a set of APIs for performing protected cryptographic operations
on STM32 internal hardware crypto accelerators using symmetric and asymmetric wrapped keys.

This software allows applications to perform protected cryptographic operations using wrapped keys that are never disclosed to the
application in cleartext. The keys are stored by the application in wrapped form. It enables the signing of the token for initial
attestation with the device attestation keys (DUA_FU, or DUA_LU) and the export of the public key.

## __Description__
This component repository contains an implementation of the middleware to interface with the PSA Crypto API as an opaque driver.

* **Supported Protected Cryptographic Operations **

<center>

| Algorithms     | Key Import | Key Generate | Encryption | Decryption | Key Agreement | Signature | Pub Key Export |
| -------------- |:----------:|:------------:|:----------:|:----------:|:-------------:|:---------:|:--------------:|
| AES CBC        |    YES     |    NO        |    YES     |     YES    |     NO        |    NO     |    NO          |
| AES ECB        |    YES     |    NO        |    YES     |     YES    |     NO        |    NO     |    NO          |
| AES GCM        |    YES     |    NO        |    YES     |     YES    |     NO        |    NO     |    NO          |
| AES CCM        |    YES     |    NO        |    YES     |     YES    |     NO        |    NO     |    NO          |
| ECDH           |    YES     |    YES       |    NO      |     NO     |     YES       |    NO     |    NO          |
| ECDSA          |    YES     |    YES       |    NO      |     NO     |     NO        |    YES    |    YES         |
| RSA PKCS(v1.5) |    YES     |    NO        |    NO      |     NO     |     NO        |    YES    |    NO          |
| RSA PKCS(v2.2) |    YES     |    NO        |    NO      |     YES    |     NO        |    YES    |    NO          |

</center>

## __How to use it?__

 This can be achieved by :

  * Adding STM32 Mbed TLS Key Wrap Engine middleware to user application.

  * Adding Mbed TLS middleware to user application.

  * Adding STM32 Mbed TLS Alt middleware to user application.

  * Defining the appropriate `MBEDTLS_ENTROPY_HARDWARE_ALT` preprocessor symbol in `mbedtls_config.h` to enable hardware entropy alternative.

  * Defining the appropriate preprocessor `MBEDTLS_HAL_ENTROPY_HARDWARE_ALT` in mbedtls_alt_config.h to enable STM32 hardware entropy source.

  * Defining the appropriate preprocessor `KWE_DRIVER_ENABLED` in kwe_config.h to enable STM32 Mbed TLS Key Wrap Engine core module.

  * Defining the appropriate preprocessor `PSA_KWE_DRIVER_ENABLED` in kwe_config.h to Enable STM32 Mbed TLS Key Wrap Engine interface module to PSA Crpto API.

  * Defining the appropriate preprocessor `KWE_ASYMMETRIC_KEY_WRAP_ENABLED` in kwe_config.h to Enable STM32 Mbed TLS Key Wrap Engine for asymmetric keys.

  **Use case : Generate ECDSA wrapped key for ECDSA signature and export the public key**

  * Add Middlewares: Ensure the STM32 Mbed TLS Key Wrap Engine, Mbed TLS, and STM32 Mbed TLS Alt middlewares are included in the project.

  * Define Preprocessors: Enable the necessary preprocessor symbols in the respective configuration files.

  * Add STM32Cube HAL drivers for STM32 hardware cryptographic accelerator to the project.

  * Generate and Export Key: Follow the steps to initialize the PSA cryptographic library, generate the ECDSA key pair, wrap the private key, export the public key, and clean up resources
  * Initialize the PSA Crypto Library using `psa_crypto_init()`.
  * Generate an ECDSA Key Pair: Key attributes should set to specify the key lifetime external using STM32 Mbed TLS Key Wrap Engine location : `PSA_CRYPTO_KWE_DRIVER_LOCATION`.
  * The key is then generated in wrapped forme using `psa_generate_key()`.
  * Compute the hash of the message using `psa_hash_compute()`.
  * Sign the hash using  `psa_sign_hash()`.
  * Export the Public Key using `psa_export_public_key()`.
  * Clean up resources using `psa_destroy_key()`.

## __Keywords__

   STM32, Key Wrap Engine, KWE, AES, GCM, CBC, RNG, ECC, ECDSA, ECDH, RSA, PKCS#1 v1.5, PKCS#1 v2.2,
   ENCRYPT, DECRYPT, SIGN, Mbed TLS, PSA Crypto API, Opaque Driver, Initial Attestation, Token,
   Challenge, Certificate, Unique Device ID, Device Electronic Signature, Authentication Key.
:::

::: {.col-sm-12 .col-lg-6}
# Update History

\

::: {.collapse}
<input type="checkbox" id="collapse-section5" checked aria-hidden="true">
<label for="collapse-section5" aria-hidden="true">__V1.0.4 / 03-June-2026__</label>
<div>

## Main changes

-  Add SW Security Classification file at the root of the component, SW_Security_Level.md

## Known limitations
- None

## Backward compatibility

- None

## Dependencies

- This component depends on:
  - Mbed TLS cryptography library version v3.6.6 or higher
  - STM32 Mbed TLS Alt version v1.2.0 or higher

</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section4" aria-hidden="true">
<label for="collapse-section4" aria-hidden="true">__V1.0.3 / 20-November-2025__</label>
<div>

## Main changes

- Maintenance release
  - Fix issue when additional data equal to zero

## Known limitations
- None

## Backward compatibility

- None.

## Dependencies

- This component depends on:
  - Mbed TLS cryptography library version v3.6.2 or higher
  - STM32 Mbed TLS Alt version v1.1.0 or higher

</div>
:::


::: {.collapse}
<input type="checkbox" id="collapse-section3" aria-hidden="true">
<label for="collapse-section3" aria-hidden="true">__V1.0.2 / 20-May-2025__</label>
<div>

## Main changes

- Maintenance release
  - Free ecp context after each use in KWE PSA driver interface

## Known limitations
- None

## Backward compatibility

- None.

## Dependencies

- This component depends on:
  - Mbed TLS cryptography library version v3.6.2 or higher
  - STM32 Mbed TLS Alt version v1.1.0 or higher

</div>
:::

::: {.collapse}
<input type="checkbox" id="collapse-section2" aria-hidden="true">
<label for="collapse-section2" aria-hidden="true">__V1.0.1 / 23-January-2025__</label>
<div>

## Main changes

- Maintenance release
  - Fix CCM implementation to support variable tag length
  - Enhance code style and remove unused parameters

## Known limitations
- None

## Backward compatibility

- None.

## Dependencies

- This component depends on:
  - Mbed TLS cryptography library v3.6.2
  - STM32 Mbed TLS Alt v1.1.0

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
  - STM32 Mbed TLS Alt v1.0.0

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