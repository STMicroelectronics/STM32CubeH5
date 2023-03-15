
## <b>USBPD_SRC Application Description</b>

This application is a USBPD type C Provider using Azure RTOS.
USBPD Provider: This application initialize the type C port 1 in source mode with only one PDO at 5V.

####  <b>Expected success behavior</b>
Connect UCPD cube Monitor on the VCP associated to our board (only available if USB cable is connected)
The UCPD Cube Monitor will be used to trace all the messages exchange between the ports partner.

#### <b>Error behaviors</b>

Errors are detected and the corresponding message is displayed on the UCPD Cube Monitor.

#### <b>Assumptions if any</b>

User is familiar with USB-PD 3.0 "Universal Serial BUS Power Delivery" Specification.

#### <b>Known limitations</b>
None.

### <b>Notes</b>


#### <b>ThreadX usage hints</b>

 - ThreadX uses the Systick as time base, thus it is mandatory that the HAL uses a separate time base through the TIM IPs.
 - ThreadX is configured with 1000 ticks/sec by default, this should be taken into account when using delays or timeouts at application. It is always possible to reconfigure it in the "tx_user.h", the "TX_TIMER_TICKS_PER_SECOND" define,but this should be reflected in "tx_initialize_low_level.s" file too.
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

#### <b>USBX usage hints</b>

### <b>Keywords</b>

Connectivity, USBPD, ThreadX, UART/USART, USB_PD_Lib, UCPD, Type C, USBPD

### <b>Hardware and Software environment</b>

  - This application runs on STM32H573xx devices
  - This application has been tested with STMicroelectronics STM32H573I-DK MB1677A
    and can be easily tailored to any other supported device and development board.

- STM32H573I-DK Set-up
    - Connect ST-Link cable to the PC USB port to display data on the CubeMx Monitor.
	
### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the application
 - Run CubeMx Monitor and open trace tools
 - Connect a type C sink device on the Type-C connector 1

<b>Note</b>

   The application uses the internal HSI clock as system clock source.
