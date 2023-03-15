## <b>UTILS_ReadDeviceInfo Example Description</b>

This example reads the UID, Device ID and Revision ID and saves
them into a global information buffer.

Then UID, Device ID and Revision ID will be saved in following global variables:

- aShowDeviceID: Device identifier
- aShowRevisionID: Revision identifier (This field indicates the revision of the device.
  For example, it is read as 0x1000 for Revision 1.0)
- aShowUIDWord0: UID Word0
- aShowUIDWord1: UID Word1
- aShowUIDWord2: UID Word2

### <b>Keywords</b>

System, Utils, Device info, UID, Device ID, Revision ID, Wafer, Lot number

### <b>Directory contents</b>

  - UTILS/UTILS_ReadDeviceInfo/Inc/stm32h5xx_it.h          Interrupt handlers header file
  - UTILS/UTILS_ReadDeviceInfo/Inc/main.h                  Header for main.c module
  - UTILS/UTILS_ReadDeviceInfo/Inc/stm32_assert.h          Template file to include assert_failed function  
  - UTILS/UTILS_ReadDeviceInfo/Src/stm32h5xx_it.c          Interrupt handlers
  - UTILS/UTILS_ReadDeviceInfo/Src/main.c                  Main program
  - UTILS/UTILS_ReadDeviceInfo/Src/system_stm32h5xx.c      STM32H5xx system source file


### <b>Hardware and Software environment</b>

  - This example runs on STM32H563ZITx devices.

  - This example has been tested with NUCLEO-H563ZI board and can be
    easily tailored to any other supported device and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example
 
 