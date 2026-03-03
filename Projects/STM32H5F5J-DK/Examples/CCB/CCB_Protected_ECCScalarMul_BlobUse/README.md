## <b>CCB_Protected_ECCScalarMul_BlobUse Example Description</b>

How to use the CCB peripheral to compute scalar multiplication k x P, using a special blob called "ECC key blob".

At the beginning of the main program, the HAL_Init() function is called to reset
all the peripherals, initialize the Flash interface and the systick.
Then the SystemClock_Config() function is used to configure the system
clock (SYSCLK).

The CCB peripheral configuration is ensured by the HAL_CCB_Init() function.
This function calls the HAL_CCB_MspInit() function, which implements the configuration
of the necessary CCB resources according to the hardware used (PKA, RNG and SAES) and enables the clock.

This example uses the Derived Hardware Unique Key (DHUK) as wrapping key.
The selected parameters for this example are an ECC 192 curve.
The related information is included in the ECCparam structure.
ECCKeyBlob will contain pointers to the reserved output buffers of IV, Tag, and the wrapped scalar K.
Calling HAL_CCB_ECC_WrapPrivateKey will wrap the input scalar K according to the ECC Curve parameters.
Next, the X and Y information related to the customized input point are felt in Point_In.
Point_Out will contain pointers to the reserved output buffers (X and Y).
Calling HAL_CCB_ECC_ComputeScalarMul will calculate the ECC Scalr multuplication and store the result in Point_Out.
At last, a verification check will be done between calculated Point_Out and expected point.

STM32 board LED is used to monitor the example status:

  - LED3(LED_GREEN) is ON when Point_Out match with expected point.
  - LED1(LED_RED) is slowly blinking when there is an error.

#### <b>Notes</b>

 1.  Care must be taken when using HAL_Delay(), this function provides accurate delay (in milliseconds)
     based on variable incremented in SysTick ISR. This implies that if HAL_Delay() is called from
     a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower)
     than the peripheral interrupt. Otherwise the caller ISR process will be blocked.
     To change the SysTick interrupt priority you have to use HAL_NVIC_SetPriority() function.

 2.  The example needs to ensure that the SysTick time base is always set to 1 millisecond
     to have correct HAL operation.


### <b>Keywords</b>

Security, CCB, PKA, ECC Scalar Multiplication

### <b>Directory contents</b>

        - CCB/CCB_Protected_ECCScalarMul_BlobUse/Inc/stm32h5f5j_discovery_conf.h     BSP configuration file
        - CCB/CCB_Protected_ECCScalarMul_BlobUse/Inc/stm32h5xx_hal_conf.h            HAL configuration file
        - CCB/CCB_Protected_ECCScalarMul_BlobUse/Inc/stm32h5xx_it.h                  Interrupt handlers header file
        - CCB/CCB_Protected_ECCScalarMul_BlobUse/Inc/main.h                          Header for main.c module
        - CCB/CCB_Protected_ECCScalarMul_BlobUse/Src/stm32h5xx_it.c                  Interrupt handlers
        - CCB/CCB_Protected_ECCScalarMul_BlobUse/Src/main.c                          Main program
        - CCB/CCB_Protected_ECCScalarMul_BlobUse/Src/stm32h5xx_hal_msp.c             HAL MSP module
        - CCB/CCB_Protected_ECCScalarMul_BlobUse/Src/system_stm32h5xx.c              STM32H5xx system source file


### <b>Hardware and Software environment</b>

  - This example runs on STM32H5F5LJHxQ devices.
  - This example has been tested with STM32H5F5J-DK board and can be
    easily tailored to any other supported device and development board.

### <b>How to use it ?</b>

In order to make the program work, you must do the following:

 - Open your preferred toolchain
 - Rebuild all files and load your image into target memory
 - Run the example
