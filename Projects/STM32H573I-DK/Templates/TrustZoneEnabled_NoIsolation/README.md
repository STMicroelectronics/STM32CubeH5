## <b>Templates_TrustZoneEnabled_No Isolation Example Description</b>

This project provides a reference template based on the STM32Cube HAL API that can be used
to build any firmware application when TrustZone security is activated (TZEN=B4).

This project is targeted to run on STM32H573xx devices on STM32H573I-DK boards from STMicroelectronics.

- At the beginning of the main program, the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.

- The SystemClock_Config() function is used to configure the system clock (SYSCLK).

#### <b>Description</b>

Please remember that on system with security enabled, the system always boots in secure.

User Option Bytes configuration:

Please note the internal Flash is fully secure by default in TZEN=4 and User Option Bytes
Security Watermarks should be set according to the application configuration.
Here the proper User Option Bytes setup in line with the project linker/scatter
file is as follows:

     - TZEN=B4 
     - SECWM1_STRT=0x0  SECWM1_END=0x7F  meaning all 128 sectors of Bank1 set as secure
     - SECWM2_STRT=0x1  SECWM2_END=0x0   meaning no sector of Bank2 set as secure, hence Bank2 set as non-secure

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.
 2. The application needs to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.
 3. The instruction cache (ICACHE) must be enabled by software to get a 0 wait-state execution
    from Flash memory and external memories, and reach the maximum performance.

### <b>Keywords</b>

Reference, Template, TrustZone

### <b>Directory contents</b>

	- Templates/TrustZoneEnabled_NoIsolation/Src/main.c                       Main program
	- Templates/TrustZoneEnabled_NoIsolation/Src/system_stm32h5xx_s.c         STM32H5xx system clock and peripheral security configuration file
	- Templates/TrustZoneEnabled_NoIsolation/Src/stm32h5xx_it.c               Interrupt handlers
	- Templates/TrustZoneEnabled_NoIsolation/Src/stm32h5xx_hal_msp.c          HAL MSP module
	- Templates/TrustZoneEnabled_NoIsolation/Inc/main.h                       Main program header file
	- Templates/TrustZoneEnabled_NoIsolation/Inc/stm32h573i_discovery_conf.h  BSP Configuration file
	- Templates/TrustZoneEnabled_NoIsolation/Inc/stm32h5xx_hal_conf.h         HAL Configuration file
	- Templates/TrustZoneEnabled_NoIsolation/Inc/stm32h5xx_it.h               Interrupt handlers header file


### <b>Hardware and Software environment</b>

  - This template runs on STM32H573xx devices with security enabled (TZEN=B4).
  - This template has been tested with STMicroelectronics STM32H573I-DK (MB1677)
    board and can be easily tailored to any other supported device
    and development board.

  - User Option Bytes requirement (with STM32CubeProgrammer tool)

        - TZEN=B4                             System with TrustZone-M enabled
        - SECWM1_STRT=0x0  SECWM1_END=0x7F    All 128 sectors of internal Flash Bank1 set as secure
        - SECWM2_STRT=0x1  SECWM2_END=0x0     No sector of internal Flash Bank2 set as secure, hence Bank2 non-secure


### <b>How to use it?</b>

In order to make the program work, you must do the following :

 - Make sure that the system is configured with the security enable (TZEN=4) (option byte)
 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example
