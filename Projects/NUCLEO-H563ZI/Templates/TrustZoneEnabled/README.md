## <b>Templates_TrustZoneEnabled Example Description</b>

- This project provides a reference template based on the STM32Cube HAL API that can be used
to build any firmware application when TrustZone security is activated (Option bit TZEN=B4).

- This project is targeted to run on STM32H563xx devices on NUCLEO-H563ZI boards from STMicroelectronics.

- At the beginning of the main program, the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.

- The SystemClock_Config() function is used to configure the system clock (SYSCLK).  


This project is composed of two sub-projects:  

 - One for the secure application part (Project_s)  
 - One for the non-secure application part (Project_ns).  

Please remember that on system with security enabled, the system always boots in secure and
the secure application is responsible for launching the non-secure application.  

This project mainly shows how to switch from secure application to non-secure application
thanks to the system isolation performed to split the internal Flash and internal SRAM memories
into two halves:  

 - The first half for the secure application and  
 - The second half for the non-secure application.  

User Option Bytes configuration:  

Please note the internal Flash is fully secure by default in TZEN = B4 and User Option Bytes
SECWM1_STRT/SECWM1_END and SECWM2_STRT/SECWM2_END should be set according to the application
configuration. 
Here the proper User Option Bytes setup in line with the project linker/scatter
file is as follows:

     - TZEN = B4
     - SECWM1_STRT=0x0  SECWM1_END=0x7F  meaning all 128 sectors of Bank1 set as secure
     - SECWM2_STRT=0x1  SECWM2_END=0x0   meaning no sector of Bank2 set as secure, hence Bank2 set as non-secure
	 
Any attempt by the non-secure application to access unauthorized code, memory or
peripheral generates a fault as demonstrated in non secure application by commenting some
code instructions in Secure/main.c (LED I/O release).  
This project is targeted to run on STM32H563xx device on NUCLEO-H563ZI boards from STMicroelectronics.  
The reference template project configures the maximum system clock frequency at 250Mhz in non-secure
application.

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The application needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.
 

### <b>Keywords</b>

Reference, Template, TrustZone

### <b>Directory contents</b>

      - Templates/TrustZoneEnabled/Secure/Src/main.c                     Secure Main program
      - Templates/TrustZoneEnabled/Secure/Src/secure_nsc.c               Secure Non-Secure Callable (NSC) module
      - Templates/TrustZoneEnabled/Secure/Src/stm32h5xx_hal_msp.c        Secure HAL MSP module
      - Templates/TrustZoneEnabled/Secure/Src/stm32h5xx_it.c             Secure Interrupt handlers
      - Templates/TrustZoneEnabled/Secure/Src/system_stm32h5xx_s.c       Secure STM32H5xx system clock configuration file
      - Templates/TrustZoneEnabled/Secure/Inc/main.h                     Secure Main program header file
      - Templates/TrustZoneEnabled/Secure/Inc/partition_stm32h563xx.h    STM32H5 Device System Configuration file
      - Templates/TrustZoneEnabled/Secure/Inc/stm32h5xx_hal_conf.h       Secure HAL Configuration file
      - Templates/TrustZoneEnabled/Secure/Inc/stm32h5xx_it.h             Secure Interrupt handlers header file
      - Templates/TrustZoneEnabled/Secure_nsclib/secure_nsc.h            Secure Non-Secure Callable (NSC) module header file
      - Templates/TrustZoneEnabled/NonSecure/Src/main.c                  Non-secure Main program
      - Templates/TrustZoneEnabled/NonSecure/Src/stm32h5xx_hal_msp.c     Non-secure HAL MSP module
      - Templates/TrustZoneEnabled/NonSecure/Src/stm32h5xx_it.c          Non-secure Interrupt handlers
      - Templates/TrustZoneEnabled/NonSecure/Src/system_stm32h5xx_ns.c   Non-secure STM32H5xx system clock configuration file
      - Templates/TrustZoneEnabled/NonSecure/Inc/main.h                  Non-secure Main program header file
      - Templates/TrustZoneEnabled/NonSecure/Inc/stm32h5xx_nucleo_conf.h BSP Configuration file
      - Templates/TrustZoneEnabled/NonSecure/Inc/stm32h5xx_hal_conf.h    Non-secure HAL Configuration file
      - Templates/TrustZoneEnabled/NonSecure/Inc/stm32h5xx_it.h          Non-secure Interrupt handlers header file

### <b>Hardware and Software environment</b>

  - This template runs on STM32H563xx devices with security enabled (TZEN=B4).  
  - This template has been tested with STMicroelectronics NUCLEO-H563ZI (MB1404)
    board and can be easily tailored to any other supported device
    and development board.  
	
  - User Option Bytes requirement (with STM32CubeProgrammer tool)

        - TZEN = B4                         System with TrustZone-M enabled
        - SECWM1_STRT=0x0  SECWM1_END=0x7F  meaning all 128 sectors of Bank1 set as secure
        - SECWM2_STRT=0x1  SECWM2_END=0x0   meaning no sector of Bank2 set as secure, hence Bank2 set as non-secure

### <b>How to use it ?</b>

In order to make the program work, you must do the following :

<b>IAR</b>

 - Open your toolchain
 - Open Multi-projects workspace file Project.eww
 - Set the "xxxxx_S" as active application (Set as Active)
 - Rebuild xxxxx_S project
 - Rebuild xxxxx_NS project
 - Load the secure and non-secures images into target memory (Ctrl + D)
 - Run the example

<b>MDK-ARM</b>

- Open your toolchain
- Open Multi-projects workspace file Project.uvmpw
- Select the xxxxx_S project as Active Project (Set as Active Project)
- Build xxxxx_S project
- Select the xxxxx_NS project as Active Project (Set as Active Project)
- Build xxxxx_NS project
- Load the non-secure binary (F8)
(this shall download the \MDK-ARM\xxxxx_ns\Exe\Project_ns.axf to flash memory)
- Select the xxxxx_S project as Active Project (Set as Active Project)
- Load the secure binary (F8)
(this shall download the \MDK-ARM\xxxxx_s\Exe\Project_s.axf to flash memory)
- Run the example

<b>STM32CubeIDE</b>

- Open your toolchain
- Open Multi-projects workspace file .project
- Select the xxxxx_S project and Build xxxxx_S project
- Select the xxxxx_NS project and Build xxxxx_NS project
- Select the xxxxx_NS project and select “Debug conguration”
- Double click on “STM32 Cortex-M C/C++ Application”
- Select “Startup” > “Add” >
- Select the xxxxx_S project
- Build configuration : Select Release/Debug
- Select the xxxxx_NS project in “Load Image and Symbols” window
- Click “Move Up” then “Debug”
- Run the example
