## <b>PWR_GPIO_Retention Example Description</b>

This example demonstrates how to enter Standby mode and wake up using an external reset or WKUP pin, save data to Backup SRAM before standby and retrieve it after wakeup.  
It also shows how to retain GPIO configuration across Standby mode using Backup SRAM.

- Configure GPIOA pin 3 as output  
- Save its configuration to backup SRAM  
- Use UserButton (wakeup pin 4) (connected to EXTI line 13) to exit Standby mode

**Entry to standby Mode:**  
On first boot, PA3 is set **HIGH** and its configuration is stored, then enters into Standby Mode. Before entering standby mode, the **LED3(BLUE)** is turned ON for 1 second.  
To enter the standby Mode again, the external reset button should be pressed.

**Exit from standby Mode:**  
When waking up from Standby (via USER push-button or external reset), the GPIOA configuration is restored from backup SRAM and PA3 resumes toggling every second.  
**LED1(GREEN)** toggles to indicate Run mode, and **LED3(BLUE)** just shows Standby transitions when it is entering the standby Mode only.

**LED status meanings:**  
- **LED1(GREEN) toggling:** System is running (Run mode), PA3 toggles every second.  
- **LED1(GREEN) & LED3(BLUE) OFF:** System is in Standby mode.  
- **LED3(BLUE) ON (for 1s):** GPIO and wakeup configuration done, system is about to enter Standby state.
- **LED2(RED) ON:** If any error occurred.

---

#### <b>Notes</b>

1. Care must be taken when using `HAL_Delay()`. This function provides accurate delay (in milliseconds) based on a variable incremented in SysTick ISR.  
   If `HAL_Delay()` is called from a peripheral ISR process, then the SysTick interrupt must have higher priority (numerically lower) than the peripheral interrupt.  
   Otherwise, the caller ISR process will be blocked. To change the SysTick interrupt priority, use `HAL_NVIC_SetPriority()`.

2. The example needs to ensure that the SysTick time base is always set to 1 millisecond for correct HAL operation.

---

### <b>Keywords</b>

Power, PWR, Standby mode, Interrupt, EXTI, Wakeup, Low Power, External reset

---

### <b>Directory Contents</b>

    - PWR/PWR_GPIO_Retention/Inc/stm32h5xx_nucleo_conf.h     BSP configuration file
    - PWR/PWR_GPIO_Retention/Inc/stm32h5xx_hal_conf.h        HAL configuration file
    - PWR/PWR_GPIO_Retention/Inc/stm32h5xx_it.h              Interrupt handlers header file
    - PWR/PWR_GPIO_Retention/Inc/main.h                      Header for main.c module
    - PWR/PWR_GPIO_Retention/Src/stm32h5xx_it.c              Interrupt handlers
    - PWR/PWR_GPIO_Retention/Src/main.c                      Main program
    - PWR/PWR_GPIO_Retention/Src/stm32h5xx_hal_msp.c         HAL MSP module 
    - PWR/PWR_GPIO_Retention/Src/system_stm32h5xx.c          STM32H5xx system source file

---

### <b>Hardware and Software Environment</b>

- This example runs on **STM32H5E5ZJTx** devices.  
- Tested with **NUCLEO-H5E5ZJ** board; can be tailored to other supported devices and development boards.

**NUCLEO-H5E5ZJ Set-up:**  
- LED1 connected to PA.03 pin  
- USER push-button connected to pin PC.13 (External line 13)  
- WakeUp Pin PWR_WAKEUP_PIN4 connected to PC.13

---

### <b>How to Use It</b>

To run the program:

1. Open your preferred toolchain  
2. Rebuild all files and load your image into target memory  
3. Run the example