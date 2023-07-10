## <b>OEMiROT_Boot application description</b>

This project provides an OEMiROT example. OEMiROT boot path performs authenticity and the integrity checks of the project firmware and data
images.

The core function of this application relies on the mcuboot middleware and the mbed-crypto middleware.

This project is full secure.

This project shows how to implement an OEMiROT.

To ease the development process, a prebuild command and postbuild command are integrated in the project.
The prebuild command is in charge of preparing the OEMiROT_Boot scatter file according to flash layout description.
The postbuild command is in charge of preparing the provisioning scripts and the application image
(firmware application managed by OEMiROT) configuration files in ROT_Provisioning/OEMiROT folder,
according to the flash layout and OEMiROT configuration.


The maximum system clock frequency at 250Mhz is configured.

### <b>Keywords</b>

TrustZone, OEMiROT, boot path, Root Of Trust, Security, mcuboot

### <b>Directory contents</b>

  - OEMiROT_Boot/Inc/mcuboot_config/mcuboot_config.h Mcuboot configuration file
  - OEMiROT_Boot/Inc/boot_hal_cfg.h                  Platform configuration file for OEMiROT_Boot
  - OEMiROT_Boot/Inc/boot_hal_flowcontrol.h          Header file for flow control code in boot_hal.c
  - OEMiROT_Boot/Inc/boot_hal_hash_ref.h             Header file for hash reference code in boot_hal.c
  - OEMiROT_Boot/Inc/boot_hal_imagevalid.h           Header file for image validation code in boot_hal.c
  - OEMiROT_Boot/Inc/cmsis.h                         Header file for CMSIS
  - OEMiROT_Boot/Inc/config-boot.h                   Mbed-crypto configuration file
  - OEMiROT_Boot/Inc/flash_layout.h                  Flash mapping
  - OEMiROT_Boot/Inc/low_level_flash.h               Header file for low_level_flash.c
  - OEMiROT_Boot/Inc/low_level_obkeys.h              Header file for low_level_obkeys.c
  - OEMiROT_Boot/Inc/low_level_rng.h                 Header file for low_level_rng.c
  - OEMiROT_Boot/Inc/low_level_security.h            Header file for low_level_security.c
  - OEMiROT_Boot/Inc/mpu_armv8m_drv.h                Header file for mpu_armv8m_drv.c
  - OEMiROT_Boot/Inc/region_defs.h                   RAM and FLASH regions definitions
  - OEMiROT_Boot/Inc/sha256_alt.h                    Header file for sha256_alt.c
  - OEMiROT_Boot/Inc/stm32h5xx_hal_conf.h            HAL driver configuration file
  - OEMiROT_Boot/Inc/target_cfg.h                    Header file for target start up
  - OEMiROT_Boot/Src/bl2_nv_services.c               Non Volatile services for OEMiROT_Boot
  - OEMiROT_Boot/Src/boot_hal.c                      Platform initialization
  - OEMiROT_Boot/Src/image_macros_to_preprocess_bl2.cImages definitions to preprocess for bl2
  - OEMiROT_Boot/Src/keys_map.c                      keys indirection to access keys in OBKeys area
  - OEMiROT_Boot/Src/low_level_com.c                 UART low level interface
  - OEMiROT_Boot/Src/low_level_device.c              Flash Low level device configuration
  - OEMiROT_Boot/Src/low_level_flash.c               Flash Low level interface
  - OEMiROT_Boot/Inc/low_level_obkeys.c              OBKeys Flash low level interface
  - OEMiROT_Boot/Inc/low_level_obkeys_device.c       OBKeys Flash low level device interface
  - OEMiROT_Boot/Src/low_level_rng.c                 Random generator interface
  - OEMiROT_Boot/Src/low_level_security.c            Security Low level services
  - OEMiROT_Boot/Src/mpu_armv8m_drv.c                Mpu low level interface
  - OEMiROT_Boot/Src/sha256_alt.c                    SHA256 HW crypto interface
  - OEMiROT_Boot/Src/startup_stm32h5xx.c             Startup file in c
  - OEMiROT_Boot/Src/stm32h5xx_hal_msp.c             HAL MSP module
  - OEMiROT_Boot/Src/system_stm32h5xx.c              System Init file
  - OEMiROT_Boot/Src/tick.c                          HAL Tick implementation

### <b>Hardware and Software environment</b>

  - This example runs on STM32H563xx devices with security enabled (TZEN=B4).
  - This example has been tested with STMicroelectronics NUCLEO-H563ZI (MB1404)
    board and can be easily tailored to any other supported device
    and development board.
  - To get debug print in your UART console you have to configure it using these parameters:
    Speed: 115200, Data: 8bits, Parity: None, stop bits: 1, Flow control: none.


### <b>How to use it?</b>

In order to build the OEMiROT_Boot project, you must do the following:
 - Open your preferred toolchain
 - Rebuild the project

Then refer to OEMiROT_Appli readme for example of application booted through OEMiROT boot path.

