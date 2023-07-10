## <b>Ux_Device_DFU Application Description</b>

This application provides an example of Azure RTOS USBX stack usage on STM32H573I-DK board,
it shows how to develop USB Device Firmware Upgrade "DFU" based application.

The application is designed to emulate an USB DFU device, the code provides all required device descriptors framework
and associated Class descriptor report to build a compliant USB DFU device.

At the beginning ThreadX calls the entry function tx_application_define(), at this stage, all USBx resources
are initialized, the DFU Class driver is registered and the application creates 2 threads with the same priorities :

  - app_ux_device_thread_entry (Prio : 10; PreemptionPrio : 10) used to initialize USB DRD HAL PCD driver and start the device.
  - usbx_dfu_download_thread_entry (Prio : 20; PreemptionPrio : 20) used to execute download command.

The DFU transactions are based on Endpoint 0 (control endpoint) transfer. All requests and status
control are sent/received through this endpoint.

The Internal flash memory is split as follows:
 - DFU area located in [0x08000000 : USBD_DFU_APP_DEFAULT_ADD-1]: Only read access
 - Application area located in [USBD_DFU_APP_DEFAULT_ADD : Device's end address]: Read, Write, and Erase
   access

*note 1:
In this application, two operating modes are available:
 1. DFU operating mode:
    This mode is entered after an MCU reset in case:
     - The DFU mode is forced by the user: the user presses the TAMP button.
     - No valid code found in the application area: a code is considered valid if the MSB of the initial
       Main Stack Pointer (MSP) value located in the first address of the application area is equal to
       0x22000.

 2. Run-time application mode:
    This is the normal run-time activities. A binary which toggles LED1 and LED2 on the STM32H573I-DK board "STM32H573I-DK_GPIO_IOToggle_@0x08022000.bin" is provided in Binary directory.


After each device reset (unplug the STM32H573I-DK board from PC), Plug the STM32H573I-DK board with Key User push-button button pressed to enter the DFU mode.

**note 3:
In case of composite device, we shall :
  1.  Uninstall device composite driver.
  1.  Activate the install of unsigned drives in windows.
  2. Updated STtube.inf driver by replacing this line in all sections from
    %USB\VID_0483&PID_DF11_DeviceDesc%=STTub30.Device, USB\VID_0483
    to
    %USB\VID_0483&PID_DF11_DeviceDesc%=STTub30.Device, USB\VID_0483&PID_DF11&MI_00
  3. Reinstall driver.


#### <b>Expected success behavior</b>

When plugged to PC host, the STM32H573I must be properly enumerated as an USB DFU device.
During the enumeration phase, device provides host with the requested descriptors (Device, configuration, string).
Those descriptors are used by host driver to identify the device capabilities.
Once the STM32H573I USB device successfully completed the enumeration phase, Open CubeProgrammer you can now download and upload to/from media .


#### <b>Error behaviors</b>

Host PC shows that USB device does not appear as DFU device.

#### <b>Assumptions if any</b>

User is familiar with USB 2.0 "Universal Serial BUS" Specification and DFU class Specification.

#### <b>Known limitations</b>

The remote wakeup feature is not yet implemented (used to bring the USB suspended bus back to the active condition).

### <b>Notes</b>

#### <b>ThreadX usage hints</b>

 - ThreadX uses the Systick as time base, thus it is mandatory that the HAL uses a separate time base through the TIM IPs.
 - ThreadX is configured with 100 ticks/sec by default, this should be taken into account when using delays or timeouts at application. It is always possible to reconfigure it in the "tx_user.h", the "TX_TIMER_TICKS_PER_SECOND" define,but this should be reflected in "tx_initialize_low_level.s" file too.
 - ThreadX is disabling all interrupts during kernel start-up to avoid any unexpected behavior, therefore all system related calls (HAL, BSP) should be done either at the beginning of the application or inside the thread entry functions.
 - ThreadX offers the "tx_application_define()" function, that is automatically called by the tx_kernel_enter() API.
   It is highly recommended to use it to create all applications ThreadX related resources (threads, semaphores, memory pools...)  but it should not in any way contain a system API call (HAL or BSP).
 - Using dynamic memory allocation requires to apply some changes to the linker file.
   ThreadX needs to pass a pointer to the first free memory location in RAM to the tx_application_define() function,
   using the "first_unused_memory" argument.
   This require changes in the linker files to expose this memory location.
    + For EWARM add the following section into the .icf file:
     ```
	 place in RAM_region    { last section FREE_MEM };
	 ```
    + For MDK-ARM:
	```
    either define the RW_IRAM1 region in the ".sct" file
    or modify the line below in "tx_low_level_initilize.s to match the memory region being used
        LDR r1, =|Image$$RW_IRAM1$$ZI$$Limit|
	```
    + For STM32CubeIDE add the following section into the .ld file:
	```
    ._threadx_heap :
      {
         . = ALIGN(8);
         __RAM_segment_used_end__ = .;
         . = . + 64K;
         . = ALIGN(8);
       } >RAM_D1 AT> RAM_D1
	```

       The simplest way to provide memory for ThreadX is to define a new section, see ._threadx_heap above.
       In the example above the ThreadX heap size is set to 64KBytes.
       The ._threadx_heap must be located between the .bss and the ._user_heap_stack sections in the linker script.
       Caution: Make sure that ThreadX does not need more than the provided heap memory (64KBytes in this example).
       Read more in STM32CubeIDE User Guide, chapter: "Linker script".

    + The "tx_initialize_low_level.s" should be also modified to enable the "USE_DYNAMIC_MEMORY_ALLOCATION" flag.


### <b>Keywords</b>

RTOS, ThreadX, USBXDevice, USB_DRD, Full Speed, DFU, USBPD, Firmware upgrade

### <b>Hardware and Software environment</b>

  - This example runs on STM32H573I devices.
  - This example has been tested with STMicroelectronics STM32H573I-DK board Revision MB1677-H573I-A01 and can be easily tailored to any other supported device and development board.

  - STM32H573I-DK board Set-up
    - Connect the STM32H573I-DK board to the PC.
    - Connect the STM32H573I-DK board to the PC through micro A-Male to standard A Male cable connected to the connector.

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - To run the application, proceed as follows:
   - Install "STM32CubeProgrammer"
   - Open "STM32CubeProgrammer", choose the "STM32H573I-DK_GPIO_IOToggle_@0x08022000.bin" provided in Binary
     directory, upgrade and verify to check that it is successfully downloaded.
   - This application allows also to upload a bin file (either the provided bin file or by creating a new bin file).
   - The downloaded application will be running just after the download.
