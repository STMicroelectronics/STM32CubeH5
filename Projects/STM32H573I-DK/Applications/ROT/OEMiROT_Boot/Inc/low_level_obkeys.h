/**
  ******************************************************************************
  * @file    low_level_obkeys.h
  * @author  MCD Application Team
  * @brief   Header for low_level_obkeys.c module
  *
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
#ifndef LOW_LEVEL_OBKEYS_H
#define LOW_LEVEL_OBKEYS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h5xx_hal.h"
#include "mcuboot_config/mcuboot_config.h"
#include "../../platform/include/tfm_plat_nv_counters.h"
#include "Driver_Flash.h"
#include "flash_layout.h"

/* Exported constants --------------------------------------------------------*/
#if  defined(OEMUROT_ENABLE)
#define OBK_HDPL2_CFG_OFFSET      (OBK_HDPL2_OFFSET) /* First OBkey Hdpl 2 for updatable config */
#define OBK_HDPL2_CFG_SIZE        (sizeof(OBK_Hdpl2Config)) /* Size for OBkey Hdpl 2 cfg section */
#define OBK_HDPL2_DATA_OFFSET     (OBK_HDPL2_CFG_OFFSET + OBK_HDPL2_CFG_SIZE) /* First OBkey Hdpl 2 for data section */
#define OBK_HDPL2_DATA_SIZE       (sizeof(OBK_Hdpl2Data)) /* Size for OBKey Hdpl 1 data section */
#else
#define OBK_HDPL1_RES_SIZE        (0x60U) /* Size reserved at start of OBKey Hdpl 1 for RSS DA */
#define OBK_HDPL1_CFG_OFFSET      (OBK_HDPL1_OFFSET + OBK_HDPL1_RES_SIZE) /* First OBkey Hdpl 1 for immutable config */
#define OBK_HDPL1_CFG_SIZE        (sizeof(OBK_Hdpl1Config)) /* Size for OBkey Hdpl 1 cfg section */
#define OBK_HDPL1_DATA_OFFSET     (OBK_HDPL1_CFG_OFFSET + OBK_HDPL1_CFG_SIZE) /* First OBkey Hdpl 1 for data section */
#define OBK_HDPL1_DATA_SIZE       (sizeof(OBK_Hdpl1Data)) /* Size for OBKey Hdpl 1 data section */
#endif

#define OBK_FLASH_PROG_UNIT       (0x10U)
#define ALL_OBKEYS                (0x1FFU)             /* Swap all OBkeys */

#define EC256_PRIV_KEY_LENGTH     (70U)
#define EC256_PUB_KEY_LENGTH      (91U)
#define SHA256_LENGTH             (32U)

#if   (CRYPTO_SCHEME == CRYPTO_SCHEME_EC256)
#define AUTH_PUB_KEY_LENGTH       EC256_PUB_KEY_LENGTH
#define ENC_PRIV_KEY_LENGTH       EC256_PRIV_KEY_LENGTH
#else
#error "undefined crypto scheme"
#endif
#define RESERVED1                 ((4U - (AUTH_PUB_KEY_LENGTH % 4U)) % 4U)
#define RESERVED2                 ((4U - (AUTH_PUB_KEY_LENGTH % 4U)) % 4U)
#define RESERVED3                 ((4U - (ENC_PRIV_KEY_LENGTH % 4U)) % 4U)
#define RESERVED4                 ((OBK_FLASH_PROG_UNIT -\
                                   ((AUTH_PUB_KEY_LENGTH + RESERVED1 +\
                                     AUTH_PUB_KEY_LENGTH + RESERVED2 +\
                                     ENC_PRIV_KEY_LENGTH + RESERVED3) % OBK_FLASH_PROG_UNIT))\
                                     % OBK_FLASH_PROG_UNIT)

/* Exported types ------------------------------------------------------------*/

#if  defined(OEMUROT_ENABLE)
/* Hdpl 2 updatable section : to be aligned on FLASH_PROG_UNIT (16 bytes) */
typedef struct __attribute__((__packed__))
{
  uint8_t  Header[32];
  uint8_t  Hdpl3SecureAuthenticationPubKey[AUTH_PUB_KEY_LENGTH];
  uint8_t  Hdpl3NonSecureAuthenticationPubKey[AUTH_PUB_KEY_LENGTH];
  uint8_t  Hdpl3EncryptionPrivKey[ENC_PRIV_KEY_LENGTH];
  uint8_t  Protected_TLV[12];
  uint8_t  Non_Protected_TLV[8];
  uint8_t  SHA256[SHA256_LENGTH];
} OBK_Hdpl2Config;

/* Hdpl 2 data section : to be aligned on FLASH_PROG_UNIT (16 bytes)
   ================================================================= */
typedef struct
{
  uint8_t  SHA256[SHA256_LENGTH];           /* Mandatory to be first in the structure */
  uint32_t Image0CurVersion;
  uint32_t Image0PrevVersion;
  uint32_t Image1CurVersion;
  uint32_t Image1PrevVersion;
  uint32_t Image2CurVersion;
  uint32_t Image2PrevVersion;
  uint32_t Image3CurVersion;
  uint32_t Image3PrevVersion;
  uint8_t  Image0SHA256[SHA256_LENGTH];
  uint8_t  Image1SHA256[SHA256_LENGTH];
  uint8_t  Image2SHA256[SHA256_LENGTH];
  uint8_t  Image3SHA256[SHA256_LENGTH];
} OBK_Hdpl2Data;
#else
/* Hdpl 1 immutable section : to be aligned on FLASH_PROG_UNIT (16 bytes)
   ====================================================================== */
typedef struct
{
  uint8_t  SHA256[SHA256_LENGTH];           /* Mandatory to be first in the structure */
  __attribute__((__aligned__(4))) uint8_t Hdpl3SecureAuthenticationPubKey[AUTH_PUB_KEY_LENGTH];
  uint8_t  Reserved1[RESERVED1];            /* Alignment on 4 bytes */
  __attribute__((__aligned__(4))) uint8_t Hdpl3NonSecureAuthenticationPubKey[AUTH_PUB_KEY_LENGTH];
  uint8_t  Reserved2[RESERVED2];            /* Alignment on 4 bytes */
  __attribute__((__aligned__(4))) uint8_t Hdpl3EncryptionPrivKey[ENC_PRIV_KEY_LENGTH];
  uint8_t  Reserved3[RESERVED3];            /* Alignment on 4 bytes */
  uint8_t  Reserved4[RESERVED4];            /* Alignment on 16 bytes */
} OBK_Hdpl1Config;

/* Hdpl 1 data section : to be aligned on FLASH_PROG_UNIT (16 bytes)
   ================================================================= */
typedef struct
{
  uint8_t  SHA256[SHA256_LENGTH];           /* Mandatory to be first in the structure */
  uint32_t Image0CurVersion;
  uint32_t Image0PrevVersion;
  uint32_t Image1CurVersion;
  uint32_t Image1PrevVersion;
  uint32_t Image2CurVersion;
  uint32_t Image2PrevVersion;
  uint32_t Image3CurVersion;
  uint32_t Image3PrevVersion;
  uint8_t  Image0SHA256[SHA256_LENGTH];
  uint8_t  Image1SHA256[SHA256_LENGTH];
  uint8_t  Image2SHA256[SHA256_LENGTH];
  uint8_t  Image3SHA256[SHA256_LENGTH];
  uint8_t  Reserved[0];                     /* Alignment on 16 bytes */
} OBK_Hdpl1Data;
#endif
/* Driver configuration
   ==================== */
typedef struct
{
  uint32_t base;
  uint32_t limit;
} OBK_FlashRange;

typedef struct
{
  uint32_t nb;
  OBK_FlashRange *range;
} OBK_FlashVect;

typedef struct
{
  OBK_FlashVect erase;
  OBK_FlashVect write;
  uint32_t read_error;
}OBK_LowLevelDevice;

/* External variables --------------------------------------------------------*/
extern OBK_LowLevelDevice OBK_FLASH0_DEV;
extern ARM_DRIVER_FLASH OBK_Driver_FLASH0;
#if  defined(OEMUROT_ENABLE)
extern OBK_Hdpl2Config OBK_Hdpl2_Cfg;
#else
extern OBK_Hdpl1Config OBK_Hdpl1_Cfg;
#endif

/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
#ifdef OEMUROT_ENABLE
void OBK_ReadHdpl2Config(OBK_Hdpl2Config *pOBK_Hdpl2Cfg);
HAL_StatusTypeDef OBK_ReadHdpl2Data(OBK_Hdpl2Data *pOBK_Hdpl2Data);
HAL_StatusTypeDef OBK_UpdateHdpl2Data(OBK_Hdpl2Data *pOBK_Hdpl2Data);
void OBK_VerifyHdpl2Config(OBK_Hdpl2Config *pOBK_Hdpl2Cfg);
#else
void OBK_ReadHdpl1Config(OBK_Hdpl1Config *pOBK_Hdpl1Cfg);
HAL_StatusTypeDef OBK_ReadHdpl1Data(OBK_Hdpl1Data *pOBK_Hdpl1Data);
HAL_StatusTypeDef OBK_UpdateHdpl1Data(OBK_Hdpl1Data *pOBK_Hdpl1Data);
void OBK_VerifyHdpl1Config(OBK_Hdpl1Config *pOBK_Hdpl1Cfg);
#endif

HAL_StatusTypeDef OBK_UpdateNVCounter(enum tfm_nv_counter_t CounterId, uint32_t Counter);
HAL_StatusTypeDef OBK_GetNVCounter(enum tfm_nv_counter_t CounterId, uint32_t *pCounter);
void OBK_InitDHUK(void);

#ifdef __cplusplus
}
#endif

#endif /* LOW_LEVEL_OBKEYS_H */
