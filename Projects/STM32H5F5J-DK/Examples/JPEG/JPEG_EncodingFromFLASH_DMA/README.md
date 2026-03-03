## <b>JPEG_EncodingFromFLASH_DMA Example Description</b>

This example demonstrates how to encode a JPEG image stored in the internal FLASH using the JPEG hardware encoder in DMA mode. The encoded image is stored in RAM. Status is indicated using onboard LEDs.

#### <b>Notes</b>

- Hardware JPEG encoding using DMA (non-blocking)
- JPEG image stored in internal flash as a C array
- Encoded output in RAM (YCbCr format)
- LED status indication:
  - **Green LED:** Test ends properly (success)
  - **Red LED:** Test error
  
The JPEG hardware encodes outputs are organized in **YCbCr format**.

#### <b>How It Works</b>

1. **Initialize the JPEG encoder.**
2. **Call** `JPEG_Encode_DMA` to start encoding the JPEG buffer with DMA (non-blocking).
   - The `JPEG_Encode_DMA` function is implemented in `encode_dma.c` as a wrapper on top of the JPEG HAL driver.
3. **Status Indication:**
   - Green LED is ON if encoding is successful.
   - Red LED is ON in case of errors.


#### <b>Important Notes</b>

- **HAL_Delay() Usage:**  
  `HAL_Delay()` provides accurate millisecond delays based on a variable incremented in the SysTick ISR.  
  If `HAL_Delay()` is called from a peripheral ISR, ensure the SysTick interrupt has a higher priority (numerically lower) than the peripheral interrupt.  
  Otherwise, the ISR may be blocked.  
  Use `HAL_NVIC_SetPriority()` to adjust priorities if needed.

---

#### <b>Directory Contents</b>

| File/Folder                                         | Description                                      |
|-----------------------------------------------------|--------------------------------------------------|
| `JPEG/JPEG_EncodingFromFLASH_DMA/Inc/stm32h5xx_hal_conf.h`                     | HAL configuration file                           |
| `JPEG/JPEG_EncodingFromFLASH_DMA/Inc/stm32h5xx_it.h`                           | Interrupt handlers header file                   |
| `JPEG/JPEG_EncodingFromFLASH_DMA/Inc/stm32h5f5lj_discovery_conf.h`             | STM32H5F5LJ-DK board configuration file          |
| `JPEG/JPEG_EncodingFromFLASH_DMA/Inc/main.h`                                   | Header for main.c module                         |
| `JPEG/JPEG_EncodingFromFLASH_DMA/Inc/encode_dma.h`                             | Header for encode_dma.c module                   |
| `JPEG/JPEG_EncodingFromFLASH_DMA/Inc/image_320_240_jpg.h`                      | JPEG image (320x240) to be encoded               |
| `JPEG/JPEG_EncodingFromFLASH_DMA/Src/stm32h5xx_it.c`                           | Interrupt handlers                               |
| `JPEG/JPEG_EncodingFromFLASH_DMA/Src/main.c`                                   | Main program                                     |
| `JPEG/JPEG_EncodingFromFLASH_DMA/Src/encode_dma.c`                             | JPEG encoding (from memory) with DMA module      |
| `JPEG/JPEG_EncodingFromFLASH_DMA/Src/stm32h5xx_hal_msp.c`                      | HAL MSP module                                   |
| `JPEG/JPEG_EncodingFromFLASH_DMA/Src/system_stm32h5xx.c`                       | System source file                               |

---

#### <b>Hardware and Software Environment</b>

- **MCU:** STM32H5F5LJ
- **Board:** STM32H5F5LJ-DK
- **Toolchain:** STM32CubeIDE, IAR(recommended) or compatible ARM GCC toolchain
- **HAL/LL Drivers:** STM32CubeH5 package

This example has been tested with the STM32H5F5LJ-DK board and can be easily tailored to other supported devices and development boards.

---

#### <b>How to Use</b>

1. **Open your toolchain** IAR (EWARM).
2. **Rebuild all files**.
3. **Load your image into target memory**.
4. **Run the example**.

---

#### <b>Keywords</b>

Graphics, JPEG, Encoding, Flash memory, DMA, Hardware Decoding, YCbCr

---