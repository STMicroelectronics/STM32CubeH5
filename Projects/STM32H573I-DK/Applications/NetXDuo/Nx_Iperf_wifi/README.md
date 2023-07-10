## <b>Nx_Iperf_wifi Application Description</b>

This application is a network traffic tool for measuring TCP and UDP performance with metrics around both throughput and latency.

The IP address of the target board to connect to is displayed on the UART.
The end-user can connect to it through an HTTP client browser and can interact with the Iperf utility running on the device through the web interface.


#### <b>Expected success behavior</b>

The HTTP client browser is able to connect and execute the use-cases that are proposed on the page.
Use-case can be started, and result of this use-case can be displayed also after end user request for it.

#### <b>Error behaviors</b>

None

#### <b>Assumptions if any</b>

- The application is using the DHCP to acquire IP address, thus a DHCP server should be reachable by the board in the LAN used to test the application.


#### <b>ThreadX usage hints</b>

- ThreadX uses the Systick as time base, thus it is mandatory that the HAL uses a separate time base through the TIM IPs.
- ThreadX is configured with 1000 ticks/sec, this should be taken into account when using delays or timeouts at application. It is always possible to reconfigure it in the `tx_user.h`, the `TX_TIMER_TICKS_PER_SECOND` define, but this should be reflected in `tx_initialize_low_level.S` file too.
- ThreadX is disabling all interrupts during kernel start-up to avoid any unexpected behavior, therefore all system related calls (HAL, BSP) should be done either at the beginning of the application or inside the thread entry functions.
- ThreadX offers the `tx_application_define()` function, that is automatically called by the tx_kernel_enter() API.
  It is highly recommended to use it to create all applications ThreadX related resources (threads, semaphores, memory pools...) but it should not in any way contain a system API call (HAL or BSP).
- Using dynamic memory allocation requires to apply some changes to the linker file.
  ThreadX needs to pass a pointer to the first free memory location in RAM to the `tx_application_define()` function, using the `first_unused_memory` argument.
  This requires changes in the linker files to expose this memory location.
    - For EWARM add the following section into the .icf file:
     ```
        place in RAM_region    { last section FREE_MEM };
     ```
    - For MDK-ARM:
    either define the RW_IRAM1 region in the ".sct" file
    or modify the line below in `tx_initialize_low_level.S` to match the memory region being used
    ```
        LDR r1, =|Image$$RW_IRAM1$$ZI$$Limit|
    ```
    - For STM32CubeIDE add the following section into the .ld file:
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
    The `._threadx_heap` must be located between the `.bss` and the `._user_heap_stack sections` in the linker script.
    Caution: Make sure that ThreadX does not need more than the provided heap memory (64KBytes in this example).
    Read more in STM32CubeIDE User Guide, chapter: "Linker script".

    - The `tx_initialize_low_level.S` should be also modified to enable the `USE_DYNAMIC_MEMORY_ALLOCATION` compilation flag.

### <b>Keywords</b>

RTOS, Network, ThreadX, NetXDuo, WiFi, Station mode, Ping, Scan, Echo, HTTP, MXCHIP, SPI, STMod+

### <b>Hardware and Software environment</b>

 - To use the EMW3080B MXCHIP Wi-Fi module functionality, 2 software components are required:
   1. The module driver running on the STM32 device
   2. The module firmware running on the EMW3080B Wi-Fi module

 - The STM32H573I-DK Discovery board RevC connects to the EMW3080B MXCHIP Wi-Fi module which contains the firmware V2.1.11; to upgrade your board with the required version V2.3.4, please visit [X-WIFI-EMW3080B](https://www.st.com/en/development-tools/x-wifi-emw3080b.html), using the “EMW3080update_STM32H573I-DK-RevA-MB1400-SPI_V2.3.4” file under the V2.3.4/SPI folder.
   The module driver is available under [/Drivers/BSP/Components/mx_wifi](../../../../../Drivers/BSP/Components/mx_wifi/), and its version is indicated in the [Release_Notes.html](../../../../../Drivers/BSP/Components/mx_wifi/Release_Notes.html) file.

 - This application runs on STM32H573xx devices without security enabled (TZEN=B4).

 - This application has been tested with STMicroelectronics STM32H573I-DK (MB1677-H573I-C01)
   board and can be easily tailored to any other supported device and development board.

 - A daughter board with the WiFi module is to be plugged into the STMod+ connector CN3 of the STM32H573I-DK board.

 - The daughterboard that was used is made up of:
   - A STMicroelectronics STMod+ adapter (MB1400-C01), delivered with the EMW3080B MXCHIP Wi-Fi module firmware V2.1.11

   > Please note that module firmware version V2.1.11 is not backwards compatible with the driver V2.3.4 (the V2.1.11 module firmware is compatible with the driver versions from V2.1.11 to V2.1.13).

   To upgrade your board with the required version V2.3.4, please visit [X-WIFI-EMW3080B](https://www.st.com/en/development-tools/x-wifi-emw3080b.html),
   using the `EMW3080update_STM32H573I-DK-RevA-MB1400-SPI_V2.3.4.bin` file under the V2.3.4/SPI folder.

 - This application requires a WiFi access point to connect to
   - With a transparent Internet connectivity: No proxy, no firewall blocking the outgoing traffic.
   - Running a DHCP server delivering the IP to the board.

 - This application uses USART1 to provide a console for commands, the hyperterminal configuration is as follows:
   - BaudRate = 115200 baud
   - Word Length = 8 Bits
   - Stop Bit = 1
   - Parity = None
   - Flow control = None
   - Line endings set to LF (receive)

 - This application requires a WiFi access point to connect to:
   - With a transparent Internet connectivity: no proxy, no firewall blocking the outgoing traffic.
   - Running a DHCP server delivering the IP and DNS configuration to the board.

 - IPerf measures TCP/UDP bitrate between an Iperf client and an Iperf server.

 - This application can work as client(`Iperf -c <serverIP>`) or as a server (`Iperf -s`).

 - A third party application must run on the network to act as the Iperf server (or the Iperf client).

   A mobile phone running the "Iperf2 for Android" application has been tested.
   A PC running the "jperf 2.0.2" application has been tested.


### <b>How to use it?</b>

In order to make the program work, you must do the following:

 - WARNING: Before opening the project with any toolchain be sure your folder installation path is not too in-depth since the toolchain may report errors after building.

 - Open your preferred toolchain

 - Edit the file `Core/Inc/mx_wifi_conf.h` to enter the name of your WiFi access point (`WIFI_SSID`) to connect to and its password (`WIFI_PASSWORD`).

 - Build all files and load your image into target memory

 - Run the application

 - Open a browser window on a PC and type the target board's IP address you got from the serial terminal.
   This web page will provide an interface to the Iperf application running on the target board.

 - Select any of the options on the web page for the target board to work as TCP/UDP client(transmit test) or as TCP/UDP server(receive test).

#### <b>Example1:</b>
 After selecting <b>"Start TCP Receive Test"</b> on the web page, launch third party iperf application command (`iperf -c <Target board's IP address>`).

 This will make Target board to act as TCP Server and third-party application as TCP Client.

log output on the client side using Jperf 2.0.2 on Windows PC:

   ```
$ bin/iperf.exe -c  192.168.137.188 -i 1
------------------------------------------------------------
Client connecting to 192.168.137.188, TCP port 5001
TCP window size: 64.0 KByte (default)
------------------------------------------------------------
[  3] local 192.168.137.1 port 64429 connected with 192.168.137.188 port 5001
[ ID] Interval       Transfer     Bandwidth
[  3]  0.0- 1.0 sec   896 KBytes  7.34 Mbits/sec
[  3]  1.0- 2.0 sec   896 KBytes  7.34 Mbits/sec
[  3]  2.0- 3.0 sec   896 KBytes  7.34 Mbits/sec
[  3]  3.0- 4.0 sec   896 KBytes  7.34 Mbits/sec
[  3]  4.0- 5.0 sec   896 KBytes  7.34 Mbits/sec
[  3]  5.0- 6.0 sec   896 KBytes  7.34 Mbits/sec
[  3]  6.0- 7.0 sec   896 KBytes  7.34 Mbits/sec
[  3]  7.0- 8.0 sec   896 KBytes  7.34 Mbits/sec
[  3]  8.0- 9.0 sec   896 KBytes  7.34 Mbits/sec
[  3]  9.0-10.0 sec   896 KBytes  7.34 Mbits/sec
[  3]  0.0-10.1 sec  8.88 MBytes  7.40 Mbits/sec
   ```

log output on the server side (select "Click here" on the web page to get results):

*TCP Receive Test Done:*

*Source IP Address: 192.168.137.1*

*Test Time(milliseconds): 10040*

*Number of Packets Received: 6203*

*Number of Bytes Received: 9038704*

*Throughput(Mbps): 7*


#### <b>Example2:</b>
 After selecting <b>"Start UDP Receive Test"</b> on the web page, launch third party iperf application command (`iperf -c <Target board's IP address> -u -b 10M`).

 This will make Target board to act as UDP Server and third-party application as UDP Client.

#### <b>Example3:</b>
 Launch third party iperf application command (`iperf -s`), now edit Destination IP address with the IP address of the device where third party
 application is running and select <b>"Start TCP transmit Test"</b> on the web page.

 This will make Target board to act as TCP Client and third-party application as TCP Server.

#### <b>Example4:</b>
 Launch third party iperf application command (`iperf -s -u`), now edit Destination IP address with the IP address of the device where third party
 application is running and select <b>"Start UDP transmit Test"</b> on the web page.

 This will make Target board to act as UDP Client and third-party application as UDP Server.


- Please refer to the following link for all the Iperf tests with example [Chapter 3 Running the Demonstration](https://docs.microsoft.com/en-us/azure/rtos/netx-duo/netx-duo-iperf/chapter3)
