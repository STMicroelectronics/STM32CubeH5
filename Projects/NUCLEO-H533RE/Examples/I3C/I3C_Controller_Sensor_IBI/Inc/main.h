/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    I3C/I3C_Controller_Sensor_IBI/Inc/main.h
  * @author  MCD Application Team
  * @brief   Header for main.c module
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h5xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#if defined(__GNUC__)
#include <stdio.h>      /*rtt*/
#include <stdlib.h>     /*rtt*/
#endif
#include "stdio.h"
#include "string.h"
#include "stm32h5xx_nucleo.h"
#include "stm32h5xx_util_i3c.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
/* Target descriptor */
typedef struct {
  char *        TARGET_NAME;          /*!< Marketing Target reference */
  uint32_t      TARGET_ID;            /*!< Target Identifier on the Bus */
  uint64_t      TARGET_BCR_DCR_PID;   /*!< Concatenation value of PID, BCR and DCR of the target */
  uint8_t       STATIC_ADDR;          /*!< Static Address of the target, value found in the datasheet of the device */
  uint8_t       DYNAMIC_ADDR;         /*!< Dynamic Address of the target preset by software/application */
} TargetDesc_TypeDef;
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* Broadcast Command code */
#define Broadcast_DISEC             0x01
#define Broadcast_RSTDAA            0x06

/* Direct Command code */
#define Direct_DISEC                0x81
#define Direct_SETDASA              0x87
#define Direct_ENEC                 0x80
#define Direct_GETBCR               0x8E
#define Direct_STATUS               0x90
/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
#define COUNTOF(__BUFFER__)   (sizeof(__BUFFER__) / sizeof(*(__BUFFER__)))
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/

/* USER CODE BEGIN Private defines */
/* Define Target Identifier */
#define DEVICE_ID1                     1U

/* Uncomment this line to have an IBI every 50 ms, if not the sensor sends an IBI every 1000 ms */
//#define DISABLE_PRINTF

/* Define LSM6DSV16X Static Address */
#define LSM6DSV16X_STA                 (0xD6>>1)
/* 8 bits address must be shift in the left for 7bits usage in interface */

/* Define LSM6DSV16X registers address */
#define LSM6DSV16X_FIFO_CTRL1_ADDRESS          0x07        /* FIFO control register 1 */
#define LSM6DSV16X_FIFO_CTRL2_ADDRESS          0x08        /* FIFO control register 2 */
#define LSM6DSV16X_FIFO_CTRL3_ADDRESS          0x09        /* FIFO control register 2 */
#define LSM6DSV16X_FIFO_CTRL4_ADDRESS          0x0A        /* FIFO control register 4 */
#define LSM6DSV16X_INT1_CTRL_ADDRESS           0x0D        /* INT1 pin control register */
#define LSM6DSV16X_WHO_AM_I_ADDRESS            0x0F        /* WHO_AM_I register */
#define LSM6DSV16X_CTRL1_ADDRESS               0x10        /* Accelerometer control register 1 */
#define LSM6DSV16X_CTRL2_ADDRESS               0x11        /* Gyroscope control register 2 */
#define LSM6DSV16X_CTRL3_ADDRESS               0x12        /* Control register 3 */
#define LSM6DSV16X_FIFO_STATUS1_ADDRESS        0x1B        /* FIFO status register 1 */
#define LSM6DSV16X_FIFO_STATUS2_ADDRESS        0x1C        /* FIFO status register 2 */
#define LSM6DSV16X_FIFO_DATA_OUT_TAG_ADDRESS   0x78        /* FIFO tag register */
#define LSM6DSV16X_FIFO_DATA_OUT_X_L_ADDRESS   0x79        /* FIFO data output X */

/* Define LSM6DSV16X registers value */
#define LSM6DSV16X_FIFO_CTRL4_VALUE            0x36     /* Config FIFO mode :continuous mode and Selects write frequency in FIFO for temperature: 15Hz */
#define LSM6DSV16X_INT1_CTRL_VALUE             0x20     /* Enable FIFO Full interrupt */
#define LSM6DSV16X_FIFO_CTRL2_VALUE            0x80     /* Config FIFO stop: FIFO depth is limited to threshold level */
#define LSM6DSV16X_FIFO_CTRL1_VALUE            250      /* Config FIFO watermark threshold */
#define LSM6DSV16X_CTRL3_VALUE                 0x44     /* enable auto incremented and enable BDU */

#if ! defined(DISABLE_PRINTF)
#define LSM6DSV16X_CTRL2_VALUE                 0x06     /* Enable and config Gyroscope: set high-performance mode and 120 Hz ODR */
#define LSM6DSV16X_CTRL1_VALUE                 0x06     /* Enable and config Accelerometer: set high-performance mode and 120 Hz ODR */
#define LSM6DSV16X_FIFO_CTRL3_VALUE            0x66     /* Config write frequency in FIFO: for gyro 120 Hz and for accel 120 Hz */
#else
#define LSM6DSV16X_CTRL2_VALUE                 0x0A     /* Enable and config Gyroscope: set high-performance mode and 1.92 kHz ODR */
#define LSM6DSV16X_CTRL1_VALUE                 0x0A     /* Enable and config Accelerometer: set high-performance mode and 1.92 kHz ODR */
#define LSM6DSV16X_FIFO_CTRL3_VALUE            0xAA     /* Config write frequency in FIFO: for gyro 1.92 kHz and for accel 1.92 kHz */
#endif /* DISABLE_PRINTF */
/* Define display refrash delay*/
#define DISPLAY_REFRESH_DELAY          100 /* Time between two display refresh in ms */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
