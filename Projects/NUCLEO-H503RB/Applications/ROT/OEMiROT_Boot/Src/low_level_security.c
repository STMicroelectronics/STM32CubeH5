/**
  ******************************************************************************
  * @file    low_level_security.c
  * @author  MCD Application Team
  * @brief   security protection implementation for secure boot on STM32H5xx
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
/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "boot_hal_cfg.h"
#include "boot_hal_flowcontrol.h"
#include "mpu_armv8m_drv.h"
#include "region_defs.h"
#include "mcuboot_config/mcuboot_config.h"
#include "low_level_security.h"
#ifdef OEMIROT_DEV_MODE
#define BOOT_LOG_LEVEL BOOT_LOG_LEVEL_INFO
#else
#define BOOT_LOG_LEVEL BOOT_LOG_LEVEL_OFF
#endif /* OEMIROT_DEV_MODE  */
#include "bootutil/bootutil_log.h"
#include "low_level_rng.h"
#include "target_cfg.h"
#include "bootutil_priv.h"

/** @defgroup OEMIROT_SECURITY_Private_Defines  Private Defines
  * @{
  */
/* DUAL BANK page size */
#define PAGE_SIZE FLASH_AREA_IMAGE_SECTOR_SIZE

#define PAGE_MAX_NUMBER_IN_BANK 0x7

/* OEMIROT_Boot Vector Address  */
#define OEMIROT_BOOT_VTOR_ADDR ((uint32_t)(BL2_CODE_START))

/**************************
  * Initial configuration *
  *************************/

/* MPU configuration
  ================== */
const struct mpu_armv8m_region_cfg_t region_cfg_init[] = {
  /* Region 0: Allows execution of BL2 */
  {
    0,
    FLASH_BASE_NS + FLASH_AREA_BL2_OFFSET,
    FLASH_BASE_NS + FLASH_AREA_BL2_OFFSET + FLASH_AREA_BL2_SIZE - 1,
    MPU_ARMV8M_MAIR_ATTR_CODE_IDX,
    MPU_ARMV8M_XN_EXEC_OK,
    MPU_ARMV8M_AP_RO_PRIV_ONLY,
    MPU_ARMV8M_SH_NONE,
#ifdef FLOW_CONTROL
    FLOW_STEP_MPU_I_EN_R0,
    FLOW_CTRL_MPU_I_EN_R0,
    FLOW_STEP_MPU_I_CH_R0,
    FLOW_CTRL_MPU_I_CH_R0,
#endif /* FLOW_CONTROL */
  },
  /* Region 1: Allows RW access before BL2 */
  {
    1,
    FLASH_BASE_NS,
    FLASH_BASE_NS + FLASH_AREA_BL2_OFFSET - 1,
    MPU_ARMV8M_MAIR_ATTR_DATANOCACHE_IDX,
    MPU_ARMV8M_XN_EXEC_NEVER,
    MPU_ARMV8M_AP_RW_PRIV_ONLY,
    MPU_ARMV8M_SH_NONE,
#ifdef FLOW_CONTROL
    FLOW_STEP_MPU_I_EN_R1,
    FLOW_CTRL_MPU_I_EN_R1,
    FLOW_STEP_MPU_I_CH_R1,
    FLOW_CTRL_MPU_I_CH_R1,
#endif /* FLOW_CONTROL */
  },
  /* Region 2: Allows RW access to all slots areas */
  {
    2,
    FLASH_BASE_NS + FLASH_AREA_BEGIN_OFFSET,
    FLASH_BASE_NS + FLASH_AREA_END_OFFSET - 1,
    MPU_ARMV8M_MAIR_ATTR_DATANOCACHE_IDX,
    MPU_ARMV8M_XN_EXEC_NEVER,
    MPU_ARMV8M_AP_RW_PRIV_ONLY,
    MPU_ARMV8M_SH_NONE,
#ifdef FLOW_CONTROL
    FLOW_STEP_MPU_I_EN_R2,
    FLOW_CTRL_MPU_I_EN_R2,
    FLOW_STEP_MPU_I_CH_R2,
    FLOW_CTRL_MPU_I_CH_R2,
#endif /* FLOW_CONTROL */
  },
  /* Region 4: Allows RW access to BL2 SRAM */
  {
    4,
    BL2_SRAM_AREA_BASE,
    BL2_SRAM_AREA_END,
    MPU_ARMV8M_MAIR_ATTR_DATA_IDX,
    MPU_ARMV8M_XN_EXEC_NEVER,
    MPU_ARMV8M_AP_RW_PRIV_ONLY,
    MPU_ARMV8M_SH_NONE,
#ifdef FLOW_CONTROL
    FLOW_STEP_MPU_I_EN_R4,
    FLOW_CTRL_MPU_I_EN_R4,
    FLOW_STEP_MPU_I_CH_R4,
    FLOW_CTRL_MPU_I_CH_R4,
#endif /* FLOW_CONTROL */
  },
  /* Region 5: Allows RW access to peripherals */
  {
    5,
    PERIPH_BASE_NS,
    PERIPH_BASE_NS + 0xFFFFFFF,
    MPU_ARMV8M_MAIR_ATTR_DEVICE_IDX,
    MPU_ARMV8M_XN_EXEC_NEVER,
    MPU_ARMV8M_AP_RW_PRIV_ONLY,
    MPU_ARMV8M_SH_NONE,
#ifdef FLOW_CONTROL
    FLOW_STEP_MPU_I_EN_R5,
    FLOW_CTRL_MPU_I_EN_R5,
    FLOW_STEP_MPU_I_CH_R5,
    FLOW_CTRL_MPU_I_CH_R5,
#endif /* FLOW_CONTROL */
  },
  /* Region 6: Allows execution of NSSLIB */
  {
    6,
    NSS_LIB_BASE,
    NSS_LIB_BASE + NSS_LIB_SIZE - 1,
    MPU_ARMV8M_MAIR_ATTR_CODE_IDX,
    MPU_ARMV8M_XN_EXEC_OK,
    MPU_ARMV8M_AP_RO_PRIV_ONLY,
    MPU_ARMV8M_SH_NONE,
#ifdef FLOW_CONTROL
    FLOW_STEP_MPU_I_EN_R6,
    FLOW_CTRL_MPU_I_EN_R6,
    FLOW_STEP_MPU_I_CH_R6,
    FLOW_CTRL_MPU_I_CH_R6,
#endif /* FLOW_CONTROL */
  },

  /* Region 7: Allows read access to STM32 descriptors */
  {
    7,
    STM32_DESCRIPTOR_BASE_NS,
    STM32_DESCRIPTOR_BASE_NS + STM32_DESCRIPTOR_SIZE - 1,
    MPU_ARMV8M_MAIR_ATTR_DATANOCACHE_IDX,
    MPU_ARMV8M_XN_EXEC_NEVER,
    MPU_ARMV8M_AP_RO_PRIV_ONLY,
    MPU_ARMV8M_SH_NONE,
#ifdef FLOW_CONTROL
    FLOW_STEP_MPU_I_EN_R7,
    FLOW_CTRL_MPU_I_EN_R7,
    FLOW_STEP_MPU_I_CH_R7,
    FLOW_CTRL_MPU_I_CH_R7,
#endif /* FLOW_CONTROL */
  },
};

const struct mpu_armv8m_region_cfg_t region_cfg_appli[] = {
  /* First region in this list is configured only at this stage, */
  /* the region will be activated later by NSS jump service. Following regions */
  /*  in this list are configured and activated at this stage. */

  /* Region 1: Allows execution of appli */
  {
    1,
    FLASH_BASE_NS + IMAGE_PRIMARY_PARTITION_OFFSET,
    FLASH_BASE_NS + IMAGE_PRIMARY_PARTITION_OFFSET + FLASH_PARTITION_SIZE - 1 - (~MPU_RLAR_LIMIT_Msk +1),
    MPU_ARMV8M_MAIR_ATTR_DATA_IDX,
    MPU_ARMV8M_XN_EXEC_OK,
    MPU_ARMV8M_AP_RO_PRIV_ONLY,
    MPU_ARMV8M_SH_NONE,
#ifdef FLOW_CONTROL
    FLOW_STEP_MPU_A_EN_R1,
    FLOW_CTRL_MPU_A_EN_R1,
    FLOW_STEP_MPU_A_CH_R1,
    FLOW_CTRL_MPU_A_CH_R1,
#endif /* FLOW_CONTROL */
  },
  /* Region 2: Allows RW access to end of area 0 for image confirmation (swap mode) */
  {
    2,
    FLASH_BASE_NS + IMAGE_PRIMARY_PARTITION_OFFSET + FLASH_PARTITION_SIZE - (~MPU_RLAR_LIMIT_Msk +1),
    FLASH_BASE_NS + IMAGE_PRIMARY_PARTITION_OFFSET + FLASH_PARTITION_SIZE - 1,
    MPU_ARMV8M_MAIR_ATTR_DATANOCACHE_IDX,
    MPU_ARMV8M_XN_EXEC_NEVER,
    MPU_ARMV8M_AP_RW_PRIV_ONLY,
    MPU_ARMV8M_SH_NONE,
#ifdef FLOW_CONTROL
    FLOW_STEP_MPU_A_EN_R2,
    FLOW_CTRL_MPU_A_EN_R2,
    FLOW_STEP_MPU_A_CH_R2,
    FLOW_CTRL_MPU_A_CH_R2,
#endif /* FLOW_CONTROL */
  },
#if   MCUBOOT_DATA_IMAGE_NUMBER == 1
  /* Region 3: Allows RW access to end of data image for image confirmation (swap mode) */
  {
    3,
    FLASH_BASE_NS + DATA_IMAGE_PRIMARY_PARTITION_OFFSET,
    FLASH_BASE_NS + DATA_IMAGE_PRIMARY_PARTITION_OFFSET + FLASH_DATA_PARTITION_SIZE - 1,
    MPU_ARMV8M_MAIR_ATTR_DATANOCACHE_IDX,
    MPU_ARMV8M_XN_EXEC_NEVER,
    MPU_ARMV8M_AP_RW_PRIV_ONLY,
    MPU_ARMV8M_SH_NONE,
#ifdef FLOW_CONTROL
    FLOW_STEP_MPU_A_EN_R3,
    FLOW_CTRL_MPU_A_EN_R3,
    FLOW_STEP_MPU_A_CH_R3,
    FLOW_CTRL_MPU_A_CH_R3,
#endif /* FLOW_CONTROL */
  },
#endif /* (MCUBOOT_DATA_IMAGE_NUMBER == 1) */
};


/* Product state control
   ===================== */
static const uint32_t ProductStatePrioList[] = {
        OB_PROD_STATE_OPEN,
        OB_PROD_STATE_PROVISIONING,
        OB_PROD_STATE_IROT_PROVISIONED,
        OB_PROD_STATE_CLOSED,
        OB_PROD_STATE_LOCKED
};

#define NB_PRODUCT_STATE (sizeof(ProductStatePrioList) / sizeof(uint32_t))

#if defined(MCUBOOT_EXT_LOADER)
/*********************************
 * Loader specific configuration *
 *********************************/

/* Peripherals configuration
   ========================= */
/*----------------------|  USART  |-------------------------------------*/
/* USART: USART1 + USART2 + USART3 */

/*----------------------|  SPI  |-------------------------------------*/
/* SPI: SPI1 + SPI2 + SPI3 */

/*----------------------| I2C  |---------------------------------------*/
/* I2C: I2C3 + I2C4 */

/*----------------------| I3C  |---------------------------------------*/
/* I3C: I3C1*/

/*----------------------|  USB |---------------------------------------*/
/* USB */

/*----------------------|  FDCAN  |------------------------------------*/
/* FDCAN2 */
/* Due to HW constraint, FDCAN1 and FDCAN2 shall be set as non-secure in order to grant FDCAN2 to non-secure (Bootloader). */

/*----------------------|  ICACHE  |------------------------------------*/
/* ICACHE */

/*----------------------|  IWDG |------------------------------------*/
/* IWDG */

#define TZSC_MASK_R1  (GTZC_CFGR1_USART2_Msk | GTZC_CFGR1_USART3_Msk | GTZC_CFGR1_SPI3_Msk  | GTZC_CFGR1_SPI2_Msk | \
                       GTZC_CFGR1_I3C1_Msk   | GTZC_CFGR1_IWDG_Msk)

#define TZSC_MASK_R2  (GTZC_CFGR2_USART1_Msk | GTZC_CFGR2_SPI1_Msk   | GTZC_CFGR2_USB_Msk )
#define TZSC_MASK_R3  (GTZC_CFGR3_ICACHE_REG_Msk)

/* SRAM1 configuration
   =================== */
/* SRAM1 NB super-block */
#define GTZC_MPCBB1_NB_VCTR (1U)

/* MPCBB : All SRAM block privileged only */
#define GTZC_MPCBB_ALL_PRIV (0xFFFFFFFFU)
/* MPU configuration
   ================= */
static const struct mpu_armv8m_region_cfg_t region_cfg_loader[] =
{
  /* Region 7: Extend read access to STM32 descriptors and bootloader vector table */
  {
    7,
    BOOTLOADER_BASE,
    BOOTLOADER_BASE + BOOTLOADER_SIZE - 1,
    MPU_ARMV8M_MAIR_ATTR_DATANOCACHE_IDX,
    MPU_ARMV8M_XN_EXEC_OK,
    MPU_ARMV8M_AP_RO_PRIV_ONLY,
    MPU_ARMV8M_SH_NONE,
#ifdef FLOW_CONTROL
    FLOW_STEP_MPU_L_EN_R7,
    FLOW_CTRL_MPU_L_EN_R7,
    FLOW_STEP_MPU_L_CH_R7,
    FLOW_CTRL_MPU_L_CH_R7,
#endif /* FLOW_CONTROL */
  },
};
#endif /* MCUBOOT_EXT_LOADER */

/**
  * @}
  */
/* Private function prototypes -----------------------------------------------*/
/** @defgroup OEMIROT_SECURITY_Private_Functions  Private Functions
  * @{
  */
static void mpu_init_cfg(void);
static void mpu_appli_cfg(void);
static void flash_priv_cfg(void);
static void ram_init_cfg(void);
#if defined(MCUBOOT_EXT_LOADER)
static void gtzc_loader_cfg(void);
#endif
#if defined(MCUBOOT_EXT_LOADER)
static void mpu_loader_cfg(void);
#endif /* MCUBOOT_EXT_LOADER */
static void active_tamper(void);
/**
  * @}
  */

/** @defgroup OEMIROT_SECURITY_Exported_Functions Exported Functions
  * @{
  */
#if defined(MCUBOOT_EXT_LOADER)
/**
  * @brief  Update the runtime security protections for application start
  *
  * @param  None
  * @retval None
  */
void LL_SECU_UpdateLoaderRunTimeProtections(void)
{
  /* configure GTZC to allow non secure / privileged loader execution */
  gtzc_loader_cfg();

  /* Set MPU to enable execution of secure /non secure  loader */
  mpu_loader_cfg();

}
#endif /* MCUBOOT_EXT_LOADER */
/**
  * @brief  Apply the runtime security  protections to
  *
  * @param  None
  * @note   By default, the best security protections are applied
  * @retval None
  */
void LL_SECU_ApplyRunTimeProtections(void)
{
  /* Set MPU to forbid execution outside of immutable code  */
  mpu_init_cfg();

  /* With OEMIROT_DEV_MODE , active tamper calls Error_Handler */
  active_tamper();

  /* Configure Flash Privilege access */
  flash_priv_cfg();

  /* Configure SRAM ECC */
  ram_init_cfg();
}

/**
  * @brief  Update the runtime security protections for application start
  *
  * @param  None
  * @retval None
  */
void LL_SECU_UpdateRunTimeProtections(void)
{
  /* Update MPU config for application execeution */
  mpu_appli_cfg();
}

/**
  * @brief  Check if the Static security protections are configured.
  *         Those protections are not impacted by a Reset. They are set using the Option Bytes.
  *         When the device is locked, these protections cannot be changed anymore.
  * @param  None
  * @note   By default, the best security protections are applied to the different
  *         flash sections in order to maximize the security level for the specific MCU.
  * @retval None
  */
void LL_SECU_CheckStaticProtections(void)
{
  static FLASH_OBProgramInitTypeDef flash_option_bytes_bank1 = {0};
  static FLASH_OBProgramInitTypeDef flash_option_bytes_bank2 = {0};
  static FLASH_OBProgramInitTypeDef flash_option_bytes_nsboot = {0};
#ifdef OEMIROT_ENABLE_SET_OB
  HAL_StatusTypeDef ret = HAL_ERROR;
#endif  /* OEMIROT_ENABLE_SET_OB  */
  uint32_t start;
  uint32_t end;
  uint32_t i;
  uint32_t start_bank2;
  uint32_t end_bank2;
  uint32_t val_bank2;

  /* Get NSBOOTADD value */
  flash_option_bytes_nsboot.BootConfig = OB_BOOT_NS;
  HAL_FLASHEx_OBGetConfig(&flash_option_bytes_nsboot);

  /* Get bank1 OB  */
  flash_option_bytes_bank1.Banks = FLASH_BANK_1;
  HAL_FLASHEx_OBGetConfig(&flash_option_bytes_bank1);

  /* Get bank2 OB  */
  flash_option_bytes_bank2.Banks = FLASH_BANK_2;
  HAL_FLASHEx_OBGetConfig(&flash_option_bytes_bank2);

#ifdef OEMIROT_ENABLE_SET_OB
  /* Clean the option configuration */
  flash_option_bytes_bank1.OptionType = 0;
  flash_option_bytes_bank2.OptionType = 0;
#endif /*   OEMIROT_ENABLE_SET_OB */



  /* Check if swap bank is reset */
  if ((flash_option_bytes_bank1.USERConfig & FLASH_OPTSR_SWAP_BANK) != 0)
  {
    BOOT_LOG_ERR("Unexpected value for swap bank configuration");
    Error_Handler();
  }

  /* Check non-secure boot addresses */
  if (flash_option_bytes_nsboot.BootAddr != BL2_BOOT_VTOR_ADDR)
  {
    BOOT_LOG_ERR("Unexpected value for NS BOOT Address");
    Error_Handler();
  }

#ifdef  OEMIROT_WRP_PROTECT_ENABLE
  uint32_t val;
  /* Check flash write protection */
  start = FLASH_AREA_PERSO_OFFSET / PAGE_SIZE;
  end = (FLASH_AREA_PERSO_OFFSET + FLASH_AREA_PERSO_SIZE + FLASH_AREA_BL2_SIZE -1) / PAGE_SIZE;
  val = 0;
  for (i = (start); i <= (end); i++)
  {
    val |= (1 << i);
  }
  /* Check if need protection in bank2 */
  val_bank2 = 0;
  if (val & (~(OB_WRP_SECTOR_ALL)))
  {
    val_bank2 = (val>>8U);
    val &= OB_WRP_SECTOR_ALL;
  }

  if ((flash_option_bytes_bank1.WRPState != OB_WRPSTATE_ENABLE)
      || (flash_option_bytes_bank1.WRPSector != val))
  {
    BOOT_LOG_INF("BANK 1 flash write protection group 0x%x: OB 0x%x",
                 (int)val, (int)flash_option_bytes_bank1.WRPSector);
#ifndef OEMIROT_ENABLE_SET_OB
    BOOT_LOG_ERR("Unexpected value for write protection ");
    Error_Handler();
#else
    flash_option_bytes_bank1.WRPState = OB_WRPSTATE_ENABLE;
    flash_option_bytes_bank1.WRPSector = val;

    BOOT_LOG_ERR("Unexpected value for write protection : set wrp1");
    flash_option_bytes_bank1.OptionType |= OPTIONBYTE_WRP;
#endif /* OEMIROT_ENABLE_SET_OB */
  }
  if (val_bank2){
    if ((flash_option_bytes_bank2.WRPState != OB_WRPSTATE_ENABLE)
        || (flash_option_bytes_bank2.WRPSector != val_bank2))
    {
      BOOT_LOG_INF("BANK 2 flash write protection group 0x%x: OB 0x%x",
                   (int)val_bank2, (int)flash_option_bytes_bank2.WRPSector);
#ifndef OEMIROT_ENABLE_SET_OB
      BOOT_LOG_ERR("Unexpected value for write protection ");
      Error_Handler();
#else
      flash_option_bytes_bank2.WRPState = OB_WRPSTATE_ENABLE;
      flash_option_bytes_bank2.WRPSector = val_bank2;

      BOOT_LOG_ERR("Unexpected value for write protection : set wrp2");
      flash_option_bytes_bank2.OptionType |= OPTIONBYTE_WRP;
#endif /* OEMIROT_ENABLE_SET_OB */
    }
  }

#endif /* OEMIROT_WRP_PROTECT_ENABLE */

#ifdef  OEMIROT_HDP_PROTECT_ENABLE
  /* Check secure user flash protection (HDP) */
  start = 0;
  end = (FLASH_BL2_HDP_END) / PAGE_SIZE;

  /* Check if need HDP in bank2 */
  start_bank2 = 0;
  end_bank2 = ~(PAGE_MAX_NUMBER_IN_BANK);
  if (end & (~(PAGE_MAX_NUMBER_IN_BANK)))
  {
    end_bank2 = end - PAGE_MAX_NUMBER_IN_BANK -1;
    end = PAGE_MAX_NUMBER_IN_BANK;
  }

  if ((start != flash_option_bytes_bank1.HDPStartSector)
    || (end != flash_option_bytes_bank1.HDPEndSector))
  {
    BOOT_LOG_INF("BANK 1 hide protection [%d, %d] : OB [%d, %d]",
                 (int)start,
                 (int)end,
                 (int)flash_option_bytes_bank1.HDPStartSector,
                 (int)flash_option_bytes_bank1.HDPEndSector);
#ifndef OEMIROT_ENABLE_SET_OB
    BOOT_LOG_ERR("Unexpected value for hide protection");
    Error_Handler();
#else
    BOOT_LOG_ERR("Unexpected value for hide protection : set hdp1");
    flash_option_bytes_bank1.HDPStartSector = start;
    flash_option_bytes_bank1.HDPEndSector = end;
    flash_option_bytes_bank1.OptionType |= OPTIONBYTE_HDP;
#endif  /*  OEMIROT_ENABLE_SET_OB */
  }

  if (end_bank2 != (~PAGE_MAX_NUMBER_IN_BANK))
  {
    if ((start_bank2 != flash_option_bytes_bank2.HDPStartSector)
        || (end_bank2 != flash_option_bytes_bank2.HDPEndSector))
    {
      BOOT_LOG_INF("BANK 2 hide protection [%d, %d] : OB [%d, %d]",
                   (int)start_bank2,
                   (int)end_bank2,
                   (int)flash_option_bytes_bank2.HDPStartSector,
                   (int)flash_option_bytes_bank2.HDPEndSector);
#ifndef OEMIROT_ENABLE_SET_OB
      BOOT_LOG_ERR("Unexpected value for hide protection");
      Error_Handler();
#else
      BOOT_LOG_ERR("Unexpected value for hide protection : set hdp2");
      flash_option_bytes_bank2.HDPStartSector = start_bank2;
      flash_option_bytes_bank2.HDPEndSector = end_bank2;
      flash_option_bytes_bank2.OptionType |= OPTIONBYTE_HDP;
#endif  /*  OEMIROT_ENABLE_SET_OB */
    }
  }

#else /* OEMIROT_HDP_PROTECT_ENABLE */
#endif /* OEMIROT_HDP_PROTECT_ENABLE */

#ifdef OEMIROT_SECURE_USER_SRAM2_ERASE_AT_RESET
  /* Check SRAM2 ERASE on reset */
  if ((flash_option_bytes_bank1.USERConfig2 & FLASH_OPTSR2_SRAM2_RST) != 0)
  {
    BOOT_LOG_ERR("Unexpected value for SRAM2 ERASE at Reset");
    Error_Handler();
  }
#endif /*OEMIROT_SECURE_USER_SRAM2_ERASE_AT_RESET */

#ifdef OEMIROT_SECURE_USER_SRAM2_ECC
  /* Check SRAM2 ECC */
  if ((flash_option_bytes_bank1.USERConfig2 & FLASH_OPTSR2_SRAM2_ECC) != 0)
  {
    BOOT_LOG_ERR("Unexpected value for SRAM2 ECC");
    Error_Handler();
  }
#endif /* OEMIROT_SECURE_USER_SRAM2_ECC */

#ifdef OEMIROT_SECURE_USER_SRAM1_ERASE_AT_RESET
  /* Check SRAM1 ERASE on reset */
  if ((flash_option_bytes_bank1.USERConfig2 & FLASH_OPTSR2_SRAM1_RST) != 0)
  {
    BOOT_LOG_ERR("Unexpected value for SRAM1 ERASE at Reset");
    Error_Handler();
  }
#endif /*OEMIROT_SECURE_USER_SRAM1_ERASE_AT_RESET */

#ifdef OEMIROT_SECURE_USER_SRAM1_ECC
  /* Check SRAM1 ECC */
  if ((flash_option_bytes_bank1.USERConfig2 & FLASH_OPTSR2_SRAM1_ECC) != 0)
  {
    BOOT_LOG_ERR("Unexpected value for SRAM2 ECC");
    Error_Handler();
  }
#endif /* OEMIROT_SECURE_USER_SRAM1_ECC */
#ifdef OEMIROT_ENABLE_SET_OB

  /* Configure Options Bytes */
  if ((flash_option_bytes_bank1.OptionType != 0) || (flash_option_bytes_bank2.OptionType != 0))
  {
    /* Unlock the Flash to enable the flash control register access */
    HAL_FLASH_Unlock();

    /* Unlock the Options Bytes */
    HAL_FLASH_OB_Unlock();

    if ((flash_option_bytes_bank1.OptionType) != 0)
    {
      /* Program the Options Bytes */
      ret = HAL_FLASHEx_OBProgram(&flash_option_bytes_bank1);
      if (ret != HAL_OK)
      {
        BOOT_LOG_ERR("Error while setting OB Bank1 config");
        Error_Handler();
      }
    }
    if ((flash_option_bytes_bank2.OptionType) != 0)
    {
      /* Program the Options Bytes */
      ret = HAL_FLASHEx_OBProgram(&flash_option_bytes_bank2);
      if (ret != HAL_OK)
      {
        BOOT_LOG_ERR("Error while setting OB Bank1 config");
        Error_Handler();
      }
    }

    /* Launch the Options Bytes (reset the board, should not return) */
    ret = HAL_FLASH_OB_Launch();
    if (ret != HAL_OK)
    {
      BOOT_LOG_ERR("Error while execution OB_Launch");
      Error_Handler();
    }

    /* Code should not be reached, reset the board */
    HAL_NVIC_SystemReset();
  }
#endif /* OEMIROT_ENABLE_SET_OB */

#ifdef OEMIROT_OB_BOOT_LOCK
  /* Check Boot lock protection */
  if (flash_option_bytes_nsboot.BootLock != OEMIROT_OB_BOOT_LOCK)
  {
    BOOT_LOG_INF("BootLock 0x%x", (int)flash_option_bytes_nsboot.BootLock);
    BOOT_LOG_ERR("Unexpected value for NS BOOT LOCK");
    Error_Handler();
  }
#endif /* OEMIROT_OB_BOOT_LOCK */

  /* Check Product State : boot if current Product State is greater or equal to selected Product State */
  /* Identify product state mini selected */
  for (i = 0U; i < NB_PRODUCT_STATE; i++)
  {
    if (ProductStatePrioList[i] == OEMIROT_OB_PRODUCT_STATE_VALUE)
    {
      break;
    }
  }
  if (i >= NB_PRODUCT_STATE)
  {
    Error_Handler();
  }
  /* Control if current product state is allowed */
  for (; i < NB_PRODUCT_STATE; i++)
  {
    if (ProductStatePrioList[i] == flash_option_bytes_bank1.ProductState)
    {
      break;
    }
  }
  if (i >= NB_PRODUCT_STATE)
  {
    Error_Handler();
  }
}


#if defined(MCUBOOT_EXT_LOADER)

/**
  * @brief  GTZC configuration before jumping into loader
  * @param  None
  * @retval None
  */
static void gtzc_loader_cfg(void)
{
  uint32_t i;

  /* configuration stage */
  if (uFlowStage == FLOW_STAGE_CFG)
  {
    __HAL_RCC_GTZC1_CLK_ENABLE();

    /* All bocks of SRAM1 configured non secure / privileged (default value)  */
    for (i = 0; i < GTZC_MPCBB1_NB_VCTR; i++)
    {
      /*SRAM1 -> MPCBB1*/
      GTZC_MPCBB1->PRIVCFGR[i] = GTZC_MPCBB_ALL_PRIV;
    }

    /* Execution stopped if flow control failed */
    FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_GTZC_L_EN_MPCBB1, FLOW_CTRL_GTZC_L_EN_MPCBB1);

    /* Required peripherals configured non secure (default value) / privileged */
    GTZC_TZSC1->PRIVCFGR1 = TZSC_MASK_R1;
    GTZC_TZSC1->PRIVCFGR2 = TZSC_MASK_R2;
    GTZC_TZSC1->PRIVCFGR3 = TZSC_MASK_R3;
    /* Execution stopped if flow control failed */
    FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_GTZC_L_EN_TZSC, FLOW_CTRL_GTZC_L_EN_TZSC);
  }
  /* verification stage */
  else
  {
    /* Verify all bocks of SRAM1 configured non secure / privileged */
    for (i = 0U; i < GTZC_MPCBB1_NB_VCTR; i++)
    {
      uint32_t privcfgr = GTZC_MPCBB1->PRIVCFGR[i];
      if (privcfgr != GTZC_MPCBB_ALL_PRIV)
      {
        Error_Handler();
      }
    }

    if (i == GTZC_MPCBB1_NB_VCTR)
    {
      /* Execution stopped if flow control failed */
      FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_GTZC_L_CH_MPCBB1, FLOW_CTRL_GTZC_L_CH_MPCBB1);
    }

    /* Verify required peripherals configured non secure / privileged */
    uint32_t privcfgr1 = GTZC_TZSC1->PRIVCFGR1;
    uint32_t privcfgr2 = GTZC_TZSC1->PRIVCFGR2;
    uint32_t privcfgr3 = GTZC_TZSC1->PRIVCFGR3;
    if (((privcfgr1 & TZSC_MASK_R1) != TZSC_MASK_R1) ||
        ((privcfgr2 & TZSC_MASK_R2) != TZSC_MASK_R2) ||
        ((privcfgr3 & TZSC_MASK_R3) != TZSC_MASK_R3))
    {
      Error_Handler();
    }
    else
    {
      /* Execution stopped if flow control failed */
      FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_GTZC_L_CH_TZSC, FLOW_CTRL_GTZC_L_CH_TZSC);
    }
  }
}
#endif /* MCUBOOT_EXT_LOADER && GENERATOR_LOADER_IN_SYSTEM_FLASH */


/**
  * @brief  mpu init
  * @param  None
  * @retval None
  */
static void mpu_init_cfg(void)
{
#ifdef OEMIROT_MPU_PROTECTION
  struct mpu_armv8m_dev_t dev_mpu = { MPU_BASE };
  int32_t i;

  /* configuration stage */
  if (uFlowStage == FLOW_STAGE_CFG)
  {
    /* configure secure MPU regions */
    for (i = 0; i < ARRAY_SIZE(region_cfg_init); i++)
    {
      if (mpu_armv8m_region_enable(&dev_mpu,
        (struct mpu_armv8m_region_cfg_t *)&region_cfg_init[i]) != MPU_ARMV8M_OK)
      {
        Error_Handler();
      }
      else
      {
        /* Execution stopped if flow control failed */
        FLOW_CONTROL_STEP(uFlowProtectValue, region_cfg_init[i].flow_step_enable,
                                             region_cfg_init[i].flow_ctrl_enable);
      }
    }

    /* enable secure MPU */
    mpu_armv8m_enable(&dev_mpu, PRIVILEGED_DEFAULT_DISABLE, HARDFAULT_NMI_ENABLE);
    FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_MPU_I_EN, FLOW_CTRL_MPU_I_EN);
  }
  /* verification stage */
  else
  {
    /* check secure MPU regions */
    for (i = 0; i < ARRAY_SIZE(region_cfg_init); i++)
    {
      if (mpu_armv8m_region_enable_check(&dev_mpu,
        (struct mpu_armv8m_region_cfg_t *)&region_cfg_init[i]) != MPU_ARMV8M_OK)
      {
        Error_Handler();
      }
      else
      {
        /* Execution stopped if flow control failed */
        FLOW_CONTROL_STEP(uFlowProtectValue, region_cfg_init[i].flow_step_check,
                                             region_cfg_init[i].flow_ctrl_check);
      }
    }

    /* check secure MPU */
    if (mpu_armv8m_check(&dev_mpu, PRIVILEGED_DEFAULT_DISABLE,
                      HARDFAULT_NMI_ENABLE) != MPU_ARMV8M_OK)
    {
      Error_Handler();
    }
    else
    {
      /* Execution stopped if flow control failed */
      FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_MPU_I_CH, FLOW_CTRL_MPU_I_CH);
    }
  }
#endif /* OEMIROT_MPU_PROTECTION */
}

/**
  * @brief  configure RAM ECC detection
  * @param  None
  * @retval None
  */
static void ram_init_cfg(void)
{
  RAMCFG_HandleTypeDef hramcfg1;
  hramcfg1.Instance = RAMCFG_SRAM1;
  RAMCFG_HandleTypeDef hramcfg2;
  hramcfg2.Instance = RAMCFG_SRAM2;

  /* configuration stage */
  if (uFlowStage == FLOW_STAGE_CFG)
  {
    __HAL_RCC_RAMCFG_CLK_ENABLE();

    /* RAMCFG_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(RAMCFG_IRQn, 0x0U, 0x0U);
    HAL_NVIC_EnableIRQ(RAMCFG_IRQn);

    /* Enable IT in case of Double error detection : ECC is activated through option bytes */
    __HAL_RAMCFG_ENABLE_IT(&hramcfg1, RAMCFG_IT_DOUBLEERR);
    __HAL_RAMCFG_ENABLE_IT(&hramcfg2, RAMCFG_IT_DOUBLEERR);

    /* Execution stopped if flow control failed */
    FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_RAMCFG_I_EN1, FLOW_CTRL_RAMCFG_I_EN1);
  }
  /* verification stage */
  else
  {
    if (((hramcfg1.Instance->IER & RAMCFG_IT_DOUBLEERR) != RAMCFG_IT_DOUBLEERR) ||
        ((hramcfg2.Instance->IER & RAMCFG_IT_DOUBLEERR) != RAMCFG_IT_DOUBLEERR))
    {
      Error_Handler();
    }
    else
    {
      /* Execution stopped if flow control failed */
      FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_RAMCFG_I_CH1, FLOW_CTRL_RAMCFG_I_CH1);
    }
  }
}


static void mpu_appli_cfg(void)
{
#ifdef OEMIROT_MPU_PROTECTION
  static struct mpu_armv8m_dev_t dev_mpu = { MPU_BASE };
  int32_t i;
  enum mpu_armv8m_error_t status;

  /* configuration stage */
  if (uFlowStage == FLOW_STAGE_CFG)
  {
    /* configure secure MPU regions */
    for (i = 0; i < ARRAY_SIZE(region_cfg_appli); i++)
    {
      /* First region configured should be activated by NSS JUMP service,
         execution rights given to primary code slot */
      if (i == 0)
      {
        status = mpu_armv8m_region_config_only(&dev_mpu, (struct mpu_armv8m_region_cfg_t *)&region_cfg_appli[i]);
      }
      else
      {
        status = mpu_armv8m_region_enable(&dev_mpu, (struct mpu_armv8m_region_cfg_t *)&region_cfg_appli[i]);
      }

      if (status != MPU_ARMV8M_OK)
      {
        Error_Handler();
      }
      else
      {
        /* Execution stopped if flow control failed */
        FLOW_CONTROL_STEP(uFlowProtectValue, region_cfg_appli[i].flow_step_enable,
                                             region_cfg_appli[i].flow_ctrl_enable);
      }
    }
  }
  else
  {
    /* check secure MPU regions */
    for (i = 0; i < ARRAY_SIZE(region_cfg_appli); i++)
    {
      /* First region configured should be activated by NSS JUMP service,
         execution rights given to primary code slot */
      if (i == 0)
      {
        status = mpu_armv8m_region_config_only_check(&dev_mpu, (struct mpu_armv8m_region_cfg_t *)&region_cfg_appli[i]);
      }
      else
      {
        status = mpu_armv8m_region_enable_check(&dev_mpu, (struct mpu_armv8m_region_cfg_t *)&region_cfg_appli[i]);
      }

      if (status != MPU_ARMV8M_OK)
      {
        Error_Handler();
      }
      else
      {
        /* Execution stopped if flow control failed */
        FLOW_CONTROL_STEP(uFlowProtectValue, region_cfg_appli[i].flow_step_check,
                                             region_cfg_appli[i].flow_ctrl_check);
      }
    }
  }
#endif /* OEMIROT_MPU_PROTECTION */
}
#if defined(MCUBOOT_EXT_LOADER)
static void mpu_loader_cfg(void)
{
#ifdef OEMIROT_MPU_PROTECTION
  struct mpu_armv8m_dev_t dev_mpu = { MPU_BASE };
  uint32_t i = 0U;
  /* Secure coding  : volatile variable usage to force compiler to reload SBS->CSLCKR register address */
  __IO uint32_t read_reg = (uint32_t) &SBS->CNSLCKR;

  /* configuration stage */
  if (uFlowStage == FLOW_STAGE_CFG)
  {
    /* configure additional MPU region */
    for (i = 0U; i < ARRAY_SIZE(region_cfg_loader); i++)
    {
      if (mpu_armv8m_region_enable(&dev_mpu,
        (struct mpu_armv8m_region_cfg_t *)&region_cfg_loader[i]) != MPU_ARMV8M_OK)
      {
        Error_Handler();
      }
      else
      {
        /* Execution stopped if flow control failed */
        FLOW_CONTROL_STEP(uFlowProtectValue, region_cfg_loader[i].flow_step_enable,
                          region_cfg_loader[i].flow_ctrl_enable);
      }
    }
  }
  /* verification stage */
  else
  {
    /* check secure MPU regions */
    for (i = 0U; i < ARRAY_SIZE(region_cfg_loader); i++)
    {
      if (mpu_armv8m_region_enable_check(&dev_mpu,
        (struct mpu_armv8m_region_cfg_t *)&region_cfg_loader[i]) != MPU_ARMV8M_OK)
      {
        Error_Handler();
      }
      else
      {
        /* Execution stopped if flow control failed */
        FLOW_CONTROL_STEP(uFlowProtectValue, region_cfg_loader[i].flow_step_check,
                          region_cfg_loader[i].flow_ctrl_check);
      }
    }

    /* Lock MPU config */
    __HAL_RCC_SBS_CLK_ENABLE();
    SBS->CNSLCKR |= SBS_CNSLCKR_LOCKNSMPU;
    FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_MPU_L_LCK, FLOW_CTRL_MPU_L_LCK);
    if (((* (uint32_t *)read_reg) & SBS_CNSLCKR_LOCKNSMPU) == 0U)
    {
      Error_Handler();
    }
    FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_MPU_L_LCK_CH, FLOW_CTRL_MPU_L_LCK_CH);
  }
#endif /* OEMIROT_MPU_PROTECTION */
}
#endif /* MCUBOOT_EXT_LOADER */


/**
  * @brief  configure flash privilege access
  * @param  None
  * @retval None
  */
static void flash_priv_cfg(void)
{
#ifdef OEMIROT_FLASH_PRIVONLY_ENABLE
  /* configuration stage */
  if (uFlowStage == FLOW_STAGE_CFG)
  {
    /* Configure Flash Privilege access */
    HAL_FLASHEx_ConfigPrivMode(FLASH_NSPRIV_DENIED);
    /* Execution stopped if flow control failed */
    FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_FLASH_P_EN, FLOW_CTRL_FLASH_P_EN);
  }
  /* verification stage */
  else
  {
    if (HAL_FLASHEx_GetPrivMode() != FLASH_NSPRIV_DENIED)
    {
      Error_Handler();
    }
    else
    {
      /* Execution stopped if flow control failed */
      FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_FLASH_P_CH, FLOW_CTRL_FLASH_P_CH);
    }
  }
#endif /*  OEMIROT_FLASH_PRIVONLY_ENABLE */
}



#if (OEMIROT_TAMPER_ENABLE != NO_TAMPER)
const RTC_PrivilegeStateTypeDef TamperPrivConf = {
    .rtcPrivilegeFull = RTC_PRIVILEGE_FULL_NO,
    .rtcPrivilegeFeatures = RTC_PRIVILEGE_FEATURE_NONE,
    .tampPrivilegeFull = TAMP_PRIVILEGE_FULL_YES,
    .MonotonicCounterPrivilege = TAMP_MONOTONIC_CNT_PRIVILEGE_NO,
    .backupRegisterStartZone2 = 0,
    .backupRegisterStartZone3 = 0
};
const RTC_InternalTamperTypeDef InternalTamperConf = {
    .IntTamper = RTC_INT_TAMPER_9 | RTC_INT_TAMPER_15,
    .TimeStampOnTamperDetection = RTC_TIMESTAMPONTAMPERDETECTION_DISABLE,
    .NoErase                  = RTC_TAMPER_ERASE_BACKUP_ENABLE
};
void TAMP_IRQHandler(void)
{
    NVIC_SystemReset();
}
#ifdef OEMIROT_DEV_MODE
extern volatile uint32_t TamperEventCleared;
#endif
#endif /* (OEMIROT_TAMPER_ENABLE != NO_TAMPER) */
RTC_HandleTypeDef RTCHandle;

static void active_tamper(void)
{
    fih_int fih_rc = FIH_FAILURE;
#if (OEMIROT_TAMPER_ENABLE == ALL_TAMPER)
    RTC_ActiveTampersTypeDef sAllTamper;
    /*  use random generator to feed  */
    uint32_t Seed[4]={0,0,0,0};
    uint32_t len=0;
    uint32_t j;
#endif /* (OEMIROT_TAMPER_ENABLE == ALL_TAMPER) */
#if (OEMIROT_TAMPER_ENABLE != NO_TAMPER)
    RTC_PrivilegeStateTypeDef TamperPrivConfGet;
#endif /* OEMIROT_TAMPER_ENABLE != NO_TAMPER) */
    /* configuration stage */
    if (uFlowStage == FLOW_STAGE_CFG)
    {
#if defined(OEMIROT_DEV_MODE) && (OEMIROT_TAMPER_ENABLE != NO_TAMPER)
        if (TamperEventCleared) {
            BOOT_LOG_INF("Boot with TAMPER Event Active");
#if (OEMIROT_TAMPER_ENABLE == ALL_TAMPER)
            /* avoid several re-boot in DEV_MODE with Tamper active */
            BOOT_LOG_INF("Plug the tamper cable, and reboot");
            BOOT_LOG_INF("Or");
#endif
            BOOT_LOG_INF("Build and Flash with flag #define OEMIROT_TAMPER_ENABLE NO_TAMPER\n");
            Error_Handler();
        }
#endif /*  OEMIROT_DEV_MODE && (OEMIROT_TAMPER_ENABLE != NO_TAMPER) */

        /* RTC Init */
        RTCHandle.Instance = RTC;
        RTCHandle.Init.HourFormat     = RTC_HOURFORMAT_12;
        RTCHandle.Init.AsynchPrediv   = RTC_ASYNCH_PREDIV;
        RTCHandle.Init.SynchPrediv    = RTC_SYNCH_PREDIV;
        RTCHandle.Init.OutPut         = RTC_OUTPUT_DISABLE;
        RTCHandle.Init.OutPutRemap    = RTC_OUTPUT_REMAP_NONE;
        RTCHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
        RTCHandle.Init.OutPutType     = RTC_OUTPUT_TYPE_PUSHPULL;
        RTCHandle.Init.OutPutPullUp   = RTC_OUTPUT_PULLUP_NONE;

        if (HAL_RTC_Init(&RTCHandle) != HAL_OK)
        {
            Error_Handler();
        }
#if (OEMIROT_TAMPER_ENABLE == ALL_TAMPER)
        /* generate random seed */
        mbedtls_hardware_poll(NULL, (unsigned char *)Seed, sizeof(Seed),(size_t *)&len);
        if (len == 0)
        {
            Error_Handler();
        }
        BOOT_LOG_INF("TAMPER SEED [0x%lx,0x%lx,0x%lx,0x%lx]", Seed[0], Seed[1], Seed[2], Seed[3]);
        /* Configure active tamper common parameters  */
        sAllTamper.ActiveFilter = RTC_ATAMP_FILTER_ENABLE;
        sAllTamper.ActiveAsyncPrescaler = RTC_ATAMP_ASYNCPRES_RTCCLK_32;
        sAllTamper.TimeStampOnTamperDetection = RTC_TIMESTAMPONTAMPERDETECTION_ENABLE;
        sAllTamper.ActiveOutputChangePeriod = 4;
        sAllTamper.Seed[0] = Seed[0];
        sAllTamper.Seed[1] = Seed[1];
        sAllTamper.Seed[2] = Seed[2];
        sAllTamper.Seed[3] = Seed[3];

        /* Disable all Active Tampers */
        /* No active tamper */
        for (j = 0; j < RTC_TAMP_NB; j++)
        {
            sAllTamper.TampInput[j].Enable = RTC_ATAMP_DISABLE;
        }
        sAllTamper.TampInput[1].Enable = RTC_ATAMP_ENABLE;
        sAllTamper.TampInput[1].Output = 1;
        sAllTamper.TampInput[1].NoErase =  RTC_TAMPER_ERASE_BACKUP_ENABLE;
        sAllTamper.TampInput[1].MaskFlag = RTC_TAMPERMASK_FLAG_DISABLE;
        sAllTamper.TampInput[1].Interrupt = RTC_ATAMP_INTERRUPT_ENABLE;
        /* Set active tampers */
        if (HAL_RTCEx_SetActiveTampers(&RTCHandle, &sAllTamper) != HAL_OK)
        {
            Error_Handler();
        }
        FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_TAMP_ACT_EN, FLOW_CTRL_TAMP_ACT_EN);
#else
        HAL_RTCEx_DeactivateTamper(&RTCHandle, RTC_TAMPER_ALL);
#endif  /* (OEMIROT_TAMPER_ENABLE == ALL_TAMPER) */
#if (OEMIROT_TAMPER_ENABLE != NO_TAMPER)
        /*  Internal Tamper activation  */
        /*  Enable Cryptographic IPs fault (tamp_itamp9), Backup domain voltage threshold monitoring (tamp_itamp1)*/
        if (HAL_RTCEx_SetInternalTamper(&RTCHandle,(RTC_InternalTamperTypeDef *)&InternalTamperConf)!=HAL_OK)
        {
            Error_Handler();
        }
        FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_TAMP_INT_EN, FLOW_CTRL_TAMP_INT_EN);


        /*  Set tamper configuration privileged only   */
        if (HAL_RTCEx_PrivilegeModeSet(&RTCHandle,(RTC_PrivilegeStateTypeDef *)&TamperPrivConf) != HAL_OK)
        {
            Error_Handler();
        }
        FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_TAMP_PRIV_EN, FLOW_CTRL_TAMP_PRIV_EN);

        /*  Activate Secret Erase */
        HAL_RTCEx_Erase_SecretDev_Conf(&RTCHandle,(uint32_t)TAMP_SECRETDEVICE_ERASE_BKP_SRAM);
        FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_TAMP_CFG_EN, FLOW_CTRL_TAMP_CFG_EN);
        BOOT_LOG_INF("TAMPER Activated");
#else
        HAL_RTCEx_DeactivateInternalTamper(&RTCHandle, RTC_INT_TAMPER_ALL);
#endif /* (OEMIROT_TAMPER_ENABLE != NO_TAMPER) */
    }
#if (OEMIROT_TAMPER_ENABLE != NO_TAMPER)
    /* verification stage */
    else
    {
#if (OEMIROT_TAMPER_ENABLE == ALL_TAMPER)
        /* Check active tampers */
        if ((READ_BIT(TAMP->ATOR, TAMP_ATOR_INITS) == 0U) ||
            (READ_REG(TAMP->IER) != (TAMP_IER_TAMP1IE<<1)) ||
            (READ_REG(TAMP->ATCR1) != 0x84050402U) ||
            (READ_REG(TAMP->ATCR2) != TAMP_ATCR2_ATOSEL2_0) ||
            (READ_REG(TAMP->CR1) != (0x41000000U | (TAMP_CR1_TAMP1E<<1))) ||
            (READ_REG(TAMP->CR2) != 0x00000000U))
        {
            Error_Handler();
        }
        FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_TAMP_ACT_CH, FLOW_CTRL_TAMP_ACT_CH);
#endif  /* (OEMIROT_TAMPER_ENABLE == ALL_TAMPER) */
        /*  Check Internal Tamper activation */
        if ((READ_BIT(RTC->CR, RTC_CR_TAMPTS) != InternalTamperConf.TimeStampOnTamperDetection) ||
#if (OEMIROT_TAMPER_ENABLE == ALL_TAMPER)
            (READ_REG(TAMP->CR1) != (0x41000000U | (TAMP_CR1_TAMP1E<<1))) ||
#else
            (READ_REG(TAMP->CR1) != 0x41000000U) ||
#endif /* (OEMIROT_TAMPER_ENABLE == ALL_TAMPER) */
            (READ_REG(TAMP->CR3) != 0x00000000U))
        {
            Error_Handler();
        }
        FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_TAMP_INT_CH, FLOW_CTRL_TAMP_INT_CH);


        /*  Check tamper configuration privileged only   */
        if (HAL_RTCEx_PrivilegeModeGet(&RTCHandle,(RTC_PrivilegeStateTypeDef *)&TamperPrivConfGet) != HAL_OK)
        {
            Error_Handler();
        }
        FIH_CALL(boot_fih_memequal, fih_rc,(void *)&TamperPrivConf, (void *)&TamperPrivConfGet, sizeof(TamperPrivConf));
        if (fih_not_eq(fih_rc, FIH_SUCCESS)) {
                Error_Handler();
        }
        FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_TAMP_PRIV_CH, FLOW_CTRL_TAMP_PRIV_CH);

        /*  Check Secret Erase */
        if (READ_BIT(TAMP->ERCFGR, TAMP_ERCFGR_ERCFG0) != TAMP_ERCFGR_ERCFG0)
        {
            Error_Handler();
        }
        FLOW_CONTROL_STEP(uFlowProtectValue, FLOW_STEP_TAMP_CFG_CH, FLOW_CTRL_TAMP_CFG_CH);
    }
#endif /*  OEMIROT_TAMPER_ENABLE != NO_TAMPER */
}
/**
  * @}
  */
