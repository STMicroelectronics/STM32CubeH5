
## <b>Nx_WebServer Application Description</b>

This application provides an example of Azure RTOS NetX Duo stack usage on STM32H573G-DK board, it shows how to develop Web HTTP server based application.
The application is designed to load files and dyncamic web pages stored in SD card using a Web HTTP server, the code provides all required features to build a compliant Web HTTP Server.
The main entry function tx_application_define() is called by ThreadX during kernel start, at this stage, all NetX and FileX resources are created.

 + A NX_PACKET_POOL **NxAppPool** is allocated

 + A NX_IP instance **NetXDuoEthIpInstance** using that pool is initialized

 + A NX_PACKET_POOL **WebServerPool** is allocated

 + A NX_WEB_HTTP_SERVER **HTTPServer** instance using that pool is initialized

 + The ARP, ICMP and protocols (TCP and UDP) are enabled for the NX_IP instance

 + A DHCP client is created.

 The application then creates 2 threads with different priorities:

 + **NxAppThread** (priority 10, PreemtionThreashold 10) : created with the TX_AUTO_START flag to start automatically.

 + **AppServerThread** (priority 5, PreemtionThreashold 5) : created with the TX_DONT_START flag to be started later.

 + **LedThread** (priority 15, PreemtionThreashold 15) : created with the TX_DONT_START flag to be started later.

The **NxAppThread** starts and perform the following actions:

  + Starts the DHCP client

  + Waits for the IP address resolution

  + Resumes the **AppServerThread**

The **AppServerThread**, once started:

  + Fx_media_open.

  + Starts HTTP server.

  + Each command coming from client (browser) is treated on the callback webserver_request_notify_callback.

The **LedThread**, once resumed from the dashboard:

  + Green Led is toggling & message printed on HyperTerminal.

#### <b>Expected success behavior</b>

 + The board IP address "IP@" is printed on the HyperTerminal

 + Home page is well displayed on the browser after entering the url: http://IP@/about.html

 + Dashboard is well displayed on the browser after entering the url: http://IP@/dashboard.html

 + On dashboard, You can command Green LED and watch some infos about threadx and netxduo.


#### <b>Error behaviors</b>

If the WEB HTTP server is not successfully started, the green LED stays OFF.
In case of other errors, the Web HTTP server does not operate as designed (Files stored in the SD card are not loaded in the web browser).

#### <b>Assumptions if any</b>

The uSD card must be plugged before starting the application.
The board must be in a DHCP Ethernet network.

#### <b>Known limitations</b>

Hotplug is not implemented for this example, that is, the SD card is expected to be inserted before application running.
The SDC card should contain the html pages.
#### <b>ThreadX usage hints</b>

 - ThreadX uses the Systick as time base, thus it is mandatory that the HAL uses a separate time base through the TIM IPs.
 - ThreadX is configured with 100 ticks/sec by default, this should be taken into account when using delays or timeouts at application. It is always possible to reconfigure it in the "tx_user.h", the "TX_TIMER_TICKS_PER_SECOND" define,but this should be reflected in "tx_initialize_low_level.S" file too.
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
    or modify the line below in "tx_initialize_low_level.S to match the memory region being used
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

    + The "tx_initialize_low_level.S" should be also modified to enable the "USE_DYNAMIC_MEMORY_ALLOCATION" flag.

#### <b>NetX Duo usage hints</b>

- Depending on the application scenario, the total TX and RX descriptors may need to be increased by updating respectively  the "ETH_TX_DESC_CNT" and "ETH_RX_DESC_CNT" in the "stm32h5xx_hal_conf.h", to guarantee the application correct behaviour, but this will cost extra memory to allocate.

#### <b>Keywords</b>

RTOS, ThreadX, Network, NetxDuo, Web HTTP Server, FileX, File ,SDMMC, UART


#### <b>Hardware and Software environment</b>

  - This application runs on STM32H573xx devices.
  - This application has been tested with STMicroelectronics STM32H573G-DK boards Revision: MB1520-H573I-B02
    and can be easily tailored to any other supported device and development board.

  - This application uses USART1 to display logs, the hyperterminal configuration is as follows:
      - BaudRate = 115200 baud
      - Word Length = 8 Bits
      - Stop Bit = 1
      - Parity = None
      - Flow control = None

#### <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the application
