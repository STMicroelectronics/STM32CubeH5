## <b>JPEG_DecodingFromFLASH_DMA Description</b>

This example demonstrates how to decode a JPEG image stored in internal FLASH using the JPEG hardware decoder in DMA mode on the **STM32H5F5LJ-DK** board. The decoded image is stored in RAM. Status is indicated using onboard LEDs.

#### <b>Notes</b>
1. **Initialize the JPEG decoder.**
2. **Call** `JPEG_Decode_DMA` to start decoding the JPEG buffer with DMA (non-blocking).
   - The `JPEG_Decode_DMA` function is implemented in `decode_dma.c` as a wrapper on top of the JPEG HAL driver.
3. **Status Indication:**
   - **Green LED:** ON if the test ends properly (success)
   - **Red LED:** ON in case of test errors

The JPEG hardware decoder outputs are organized in **YCbCr format**.

### <b>Keywords</b>
JPEG, DMA


#### <b>Important Notes</b>

- **HAL_Delay() Usage:**  
  `HAL_Delay()` provides accurate millisecond delays based on a variable incremented in the SysTick ISR.  
  If `HAL_Delay()` is called from a peripheral ISR, ensure the SysTick interrupt has a higher priority (numerically lower) than the peripheral interrupt.  
  Otherwise, the ISR may be blocked.  
  Use `HAL_NVIC_SetPriority()` to adjust priorities if needed.

---

#### <b>Directory Contents</b>

| File/Folder                                               | Description                                      |
|-----------------------------------------------------------|--------------------------------------------------|
| `JPEG/JPEG_DecodingFromFLASH_DMA/Inc/stm32h5xx_hal_conf.h`                           | HAL configuration file                           |
| `JPEG/JPEG_DecodingFromFLASH_DMA/Inc/stm32h5xx_it.h`                                 | Interrupt handlers header file                   |
| `JPEG/JPEG_DecodingFromFLASH_DMA/Inc/stm32h5f5lj_discovery_conf.h`                   | STM32H5F5LJ-DK board configuration file          |
| `JPEG/JPEG_DecodingFromFLASH_DMA/Inc/main.h`                                         | Header for main.c module                         |
| `JPEG/JPEG_DecodingFromFLASH_DMA/Inc/decode_dma.h`                                   | Header for decode_dma.c module                   |
| `JPEG/JPEG_DecodingFromFLASH_DMA/Inc/image_320_240_jpg.h`                            | JPEG image (320x240) to be decoded               |
| `JPEG/JPEG_DecodingFromFLASH_DMA/Src/stm32h5xx_it.c`                                 | Interrupt handlers                               |
| `JPEG/JPEG_DecodingFromFLASH_DMA/Src/main.c`                                         | Main program                                     |
| `JPEG/JPEG_DecodingFromFLASH_DMA/Src/decode_dma.c`                                   | JPEG decoding (from memory) with DMA module      |
| `JPEG/JPEG_DecodingFromFLASH_DMA/Src/stm32h5xx_hal_msp.c`                            | HAL MSP module                                   |
| `JPEG/JPEG_DecodingFromFLASH_DMA/Src/system_stm32h5xx.c`                             | System source file                               |

---

#### <b>Hardware and Software Environment</b>

- **MCU:** STM32H5F5LJ series
- **Board:** STM32H5F5LJ-DK (easily adaptable to other STM32H5 boards)
- **Toolchain:** IAR, STM32CubeIDE or compatible ARM GCC toolchain
- **HAL/LL Drivers:** STM32CubeH5 package


#### <b>How to Use</b>

1. **Open your  toolchain** IAR (EWARM).
2. **Rebuild all files**.
3. **Load your image into target memory**.
4. **Run the example**.

---
