## <b>GPIO_IOToggle_TrustZone Example Description</b>

How to use HAL GPIO to toggle secure and unsecure IOs when TrustZone security is activated (Option bit TZEN=B4).

The purpose of this example is to declare a secure IO with HAL GPIO and to toggle it
every second on Secure SysTick timer interrupt basis. All other IOs are released to
non-secure application and a non-secure IO is initialized by the non-secure application
to toggle every two seconds on Non-secure Systick timer interrupt basis.

The secure IO is PB.00 which corresponds to LED1 and the non-secure toggling IO is PF.04 for LED2.

The secure **LED1 toggles** every second and remains on in case of error in secure code.
The non-secure **LED2 toggles** twice faster and remains on in case of error in non-secure code.

This project is composed of two sub-projects:

- One for the secure application part (xxxxx_S)
- One for the non-secure application part (xxxxx_NS).

Please remember that on system with security enabled:

- the system always boots in secure and the secure application is responsible for
launching the non-secure application.
- the SystemInit() function in secure application sets up the SAU/IDAU, FPU and
secure/non-secure interrupts allocations defined in partition_stm32h563xx.h file.

This project shows how to switch from secure application to non-secure application
thanks to the system isolation performed to split the internal Flash and internal SRAM memories
into two halves:

 - The first half for the secure application and
 - The second half for the non-secure application.

User Option Bytes configuration:
Please note the internal Flash is fully secure by default in TZEN=B4 and User Option Bytes
SECWM1_PSTRT/SECWM1_PEND and SECWM2_PSTRT/SECWM2_PEND should be set according to the application
configuration. Here the proper User Option Bytes setup in line with the project linker/scatter
file is as follows:

    - TZEN=B4
    - SECWM1_PSTRT=0x0  SECWM1_PEND=0x7F  meaning all 128 sectors of Bank1 set as secure
    - SECWM2_PSTRT=0x7F SECWM2_PEND=0x0   meaning all 128 sectors of Bank2 set as non_secure

Any attempt by the non-secure application to access unauthorized code, memory or
peripheral generates a fault as demonstrated in non secure application by uncommenting some
code instructions in main.c.

This project is targeted to run on STM32H5xx device on boards from STMicroelectronics.

This example configures the maximum system clock frequency at 250 MHz.

#### <b>Notes</b>

 1. Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
    based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
    a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
    than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
    To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2. The application need to ensure that the SysTick time base is always set to 1 millisecond
    to have correct HAL operation.

### <b>Keywords</b>

System, GPIO, GTZC, Output, Alternate function, Push-pull, TrustZone

### <b>Directory contents</b>

      -  FLASH/GPIO_IOToggle_TrustZone/Secure/Src/main.c                             Secure Main program
      -  FLASH/GPIO_IOToggle_TrustZone/Secure/Src/secure_nsc.c                       Secure Non-Secure Callable (NSC) module
      -  FLASH/GPIO_IOToggle_TrustZone/Secure/Src/stm32h5xx_hal_msp.c                Secure HAL MSP module
      -  FLASH/GPIO_IOToggle_TrustZone/Secure/Src/stm32h5xx_it.c                     Secure Interrupt handlers
      -  FLASH/GPIO_IOToggle_TrustZone/Secure/Src/system_stm32h5xx_s.c               Secure STM32H5xx system clock configuration file
      -  FLASH/GPIO_IOToggle_TrustZone/Secure/Inc/main.h                             Secure Main program header file
      -  FLASH/GPIO_IOToggle_TrustZone/Secure/Inc/partition_stm32h563xx.h            STM32H5 Device System Configuration file
      -  FLASH/GPIO_IOToggle_TrustZone/Secure/Inc/stm32h5xx_hal_conf.h               Secure HAL Configuration file
      -  FLASH/GPIO_IOToggle_TrustZone/Secure/Inc/stm32h5xx_it.h                     Secure Interrupt handlers header file
      -  FLASH/GPIO_IOToggle_TrustZone/Secure_nsclib/secure_nsc.h                    Secure Non-Secure Callable (NSC) module header file
      -  FLASH/GPIO_IOToggle_TrustZone/NonSecure/Src/main.c                          Non-secure Main program
      -  FLASH/GPIO_IOToggle_TrustZone/NonSecure/Src/stm32h5xx_hal_msp.c             Non-secure HAL MSP module
      -  FLASH/GPIO_IOToggle_TrustZone/NonSecure/Src/stm32h5xx_it.c                  Non-secure Interrupt handlers
      -  FLASH/GPIO_IOToggle_TrustZone/NonSecure/Src/system_stm32h5xx_ns.c           Non-secure STM32H5xx system clock configuration file
      -  FLASH/GPIO_IOToggle_TrustZone/NonSecure/Inc/main.h                          Non-secure Main program header file
      -  FLASH/GPIO_IOToggle_TrustZone/NonSecure/Inc/stm32h5xx_nucleo_conf.h         BSP Configuration file
      -  FLASH/GPIO_IOToggle_TrustZone/NonSecure/Inc/stm32h5xx_hal_conf.h            Non-secure HAL Configuration file
      -  FLASH/GPIO_IOToggle_TrustZone/NonSecure/Inc/stm32h5xx_it.h                  Non-secure Interrupt handlers header file

### <b>Hardware and Software environment</b>

  - This example runs on STM32H563ZITx devices with security enabled (TZEN=B4).

  - This example has been tested with STMicroelectronics NUCLEO-H563ZI
    board and can be easily tailored to any other supported device
    and development board.

  - User Option Bytes requirement (with STM32CubeProgrammer tool)

        - TZEN = B4                      System with TrustZone-M enabled
        - SECWM1_STRT=0x0  SECWM1_END=0x7F  All 128 sectors of internal Flash Bank1 set as secure
        - SECWM2_STRT=0x1  SECWM2_END=0x0   No sector of internal Flash Bank2 set as secure, hence Bank2 non-secure

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

 - Open STM32CubeIDE
 - File > Import. Point to the STM32CubeIDE folder of the example project. Click Finish.
 - Select and build the xxxxx_NS project, this will automatically trigger build of xxxxx_S project
 - Select the xxxxx_S project and select “Debug configuration”
   - Double click on “STM32 Cortex-M C/C++ Application”
   - Select  “Startup” >  “Add” >
 - Select the xxxxx_NS project
 - Build configuration : Select Release/Debug
   - Click Debug to debug the example
