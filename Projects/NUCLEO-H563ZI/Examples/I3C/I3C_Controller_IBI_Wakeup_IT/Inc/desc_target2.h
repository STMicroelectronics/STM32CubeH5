/**
  ******************************************************************************
  * @file    desc_target2.h
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
#ifndef __DESC_TARGET2_H
#define __DESC_TARGET2_H

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
#define TARGET2_DYN_ADDR        0x34

/********************/
/* Target Descriptor */
/********************/
TargetDesc_TypeDef TargetDesc2 =
{
  "TARGET_ID2",
  DEVICE_ID2,
  0x0000000000000000,
  0x00,
  TARGET2_DYN_ADDR,
};

#endif /* __DESC_TARGET2_H */
