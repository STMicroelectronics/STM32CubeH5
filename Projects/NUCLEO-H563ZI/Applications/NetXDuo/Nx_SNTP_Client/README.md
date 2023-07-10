
## <b>Nx_SNTP_Client Application Description</b>

This application provides an example of Azure RTOS NetX/NetXDuo stack usage. 

It shows how to develop a NetX SNTP client and connect with an STNP server to get a time update.

The main entry function tx_application_define() is called by ThreadX during kernel start, at this stage, all NetX resources are created.

 + A NX_PACKET_POOL is allocated
 
 + A NX_IP instance using that pool is initialized
 
 + The ARP, ICMP and UDP protocols are enabled for the NX_IP instance

 + An SNTP Client "SntpClient" is created.
 
 + A DHCP client is created.
 
The application then creates 2 threads with the same priorities:

 + **AppMainThread** (priority 10, PreemtionThreashold 10) : created with the TX_AUTO_START flag to start automatically.
 
 + **AppSNTPThread** (priority 5, PreemtionThreashold 5) : created with the TX_DONT_START flag to be started later.
 
The **AppMainThread** starts and perform the following actions:
  + starts the DHCP client
  
  + waits for the IP address resolution
  
  + resumes the **AppSNTPThread**

The **AppSNTPThread**, once started:

  + creates a dns_client with USER_DNS_ADDRESS used as DNS server.

  + initialize SntpClient in Unicast mode and set SNTP_SERVER_NAME predefined in app_netxduo.h
  
  + run SntpClient
  
  + once a valid time update received, time will be displayed on the Hyperterminal and set to RTC
  
  + RTC time will be displayed each second on the Hyperterminal.
  
#### <b>Expected success behavior</b>

 + The board IP address is printed on the HyperTerminal
 + The time update sent by the sntp server is printed on the HyerTerminal
 + Each second, time from RTC is printed on the HyerTerminal and the green LED is toggling.

#### <b>Error behaviors</b>

+ The red LED is toggling to indicate any error that has occurred.

#### <b>Assumptions if any</b>

- The Application is using the DHCP to acquire IP address, thus a DHCP server should be reachable by the board in the LAN used to test the application.
- The application is configuring the Ethernet IP with a static predefined _MAC Address_, make sure to change it in case multiple boards are connected on the same LAN to avoid any potential network traffic issues.

- The _MAC Address_ is defined in the `main.c`

```
void MX_ETH_Init(void)
{

  /* USER CODE BEGIN ETH_Init 0 */

  /* USER CODE END ETH_Init 0 */

  /* USER CODE BEGIN ETH_Init 1 */

  /* USER CODE END ETH_Init 1 */
  heth.Instance = ETH;
  heth.Init.MACAddr[0] =   0x00;
  heth.Init.MACAddr[1] =   0x11;
  heth.Init.MACAddr[2] =   0x83;
  heth.Init.MACAddr[3] =   0x45;
  heth.Init.MACAddr[4] =   0x26;
  heth.Init.MACAddr[5] =   0x11;
```
#### <b>Known limitations</b>

default NX_SNTP_CLIENT_MAX_ROOT_DISPERSION and NX_SNTP_CLIENT_MIN_SERVER_STRATUM values in "nx_user.h" may not work for some SNTP servers, they should be tuned for example :

#define NX_SNTP_CLIENT_MAX_ROOT_DISPERSION    500000

#define NX_SNTP_CLIENT_MIN_SERVER_STRATUM     5

### <b>Notes</b>

  
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

### <b>Keywords</b>

RTOS, Network, ThreadX, NetXDuo, SNTP, UART

### Hardware and Software environment</b>

 - This application runs on STM32H563xx devices.
 - This application has been tested with STMicroelectronics NUCLEO-H563ZI boards Revision: MB1404-H563ZI-A01
    and can be easily tailored to any other supported device and development board.

 - This application uses USART1 to display logs, the hyperterminal configuration is as follows:
      - BaudRate = 115200 baud
      - Word Length = 8 Bits
      - Stop Bit = 1
      - Parity = None
      - Flow control = None

###  <b>How to use it ?</b>

In order to make the program work, you must do the following :

 - Open your preferred toolchain
 - Edit the file app_netxduo.h : define the USER_DNS_ADDRESS, define the SNTP server name to connect to.
 - Rebuild all files and load your image into target memory
 - Run the application