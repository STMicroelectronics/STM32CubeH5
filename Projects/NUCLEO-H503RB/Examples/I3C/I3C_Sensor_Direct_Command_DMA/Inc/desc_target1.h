/**
  ******************************************************************************
  * @file    desc_target1.h
  * @author  MCD Application Team
  * @brief   This file contain the descriptor of an I3C target.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32_I3C_DESC_TARGET1_H
#define __STM32_I3C_DESC_TARGET1_H

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#define TARGET1_DYN_ADDR        0x32

/********************/
/* Target Descriptor */
/********************/
TargetDesc_TypeDef TargetDesc1 =
{
  "TARGET_ID1",
  DEVICE_ID1,
  0x0000000000000000,
  0x00,
  TARGET1_DYN_ADDR,
};

#endif /* __STM32_I3C_DESC_TARGET1_H */
