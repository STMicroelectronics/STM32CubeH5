
---
pagetitle: Readme
lang: en
---
::: {.row}
::: {.col-sm-12 .col-lg-8}

## <b>STiROT_OEMuROT_Appli application Description</b>

For STiROT_OEMuROT boot path, no dedicated project are provided. The projects OEMiROT_Boot and OEMiROT_Appli_TrustZone are used to demonstrate this boot path.

### <b>Keywords</b>

TrustZone, STiROT, OEMuROT, Boot path, Root Of Trust, Security, mcuboot

### <b>Hardware and Software environment</b>

  - This example runs on STM32H573xx devices with security enabled (TZEN=B4).
  - This example has been tested with STMicroelectronics STM32H573I-DK (MB1677)
    board and can be easily tailored to any other supported device
    and development board.
  - To get debug print in your UART console you have to configure it using these parameters:
    Speed: 115200, Data: 8bits, Parity: None, stop bits: 1, Flow control: none.

### <b>How to use it?</b>

To run this application, execute the provisioning script of STiROT_OEMuROT boot path (ROT_Provisioning/STiROT_OEMuROT folder), and follow instructions of the script.
During this phase, the OEMiROT_Boot and OEMiROT_Appli_TrustZone need to be built, refer to their readme for more details.


:::
:::

