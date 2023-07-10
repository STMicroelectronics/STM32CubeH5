/**
  ******************************************************************************
  * @file    test_versions.c
  * @author  MCD Application Team
  * @brief   image versions extraction
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
#include <stdio.h>
#include "stm32h5xx_hal.h"
#include "test_versions.h"
#include "low_level_obkeys.h"
#include "main.h"

#ifdef DATA_IMAGE_EN
#define DATA_PRIMARY_OFFSET           (0x00000900)
#define DATA_PRIMARY_SIZE             (0x00000200)
#endif

/* Private variables ---------------------------------------------------------*/
/* Global variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Functions Definition ------------------------------------------------------*/

/**
  * @brief  Display the image Versions on HyperTerminal
  * @param  None.
  * @retval None.
  */
void TEST_VERSIONS_PrintVersions(void)
{
  struct image_header *AppliCodeHeader;

  /* Read appli code header */
  AppliCodeHeader = (struct image_header *)(APPLI_CODE_HEADER);

  /* Print version with same format than mcuboot log */
  printf("\r\n Appli Code version: %d.%d.%d\r\n", AppliCodeHeader->ih_ver.iv_major,
         AppliCodeHeader->ih_ver.iv_minor,
         AppliCodeHeader->ih_ver.iv_revision);
#ifdef DATA_IMAGE_EN
  struct image_header *AppliDataHeader;
  uint8_t buffer[sizeof(struct image_header)];
  /* Read appli data header */
  if (OBK_Flash_ReadEncrypted(DATA_PRIMARY_OFFSET, buffer, sizeof(buffer)) != ARM_DRIVER_OK)
  {
    printf("\r\n Error reading Appli Data version\r\n");
  }
  else
  {
    AppliDataHeader = (struct image_header *)buffer;

    /* Print version with same format than mcuboot log */
    printf(" Appli Data version: %d.%d.%d\r\n", AppliDataHeader->ih_ver.iv_major,
           AppliDataHeader->ih_ver.iv_minor,
           AppliDataHeader->ih_ver.iv_revision);
  }
#endif /* #ifdef DATA_IMAGE_EN */
}

/**
  * @brief  Display the S Data on HyperTerminal
  * @param  None.
  * @retval None.
  */
#ifdef DATA_IMAGE_EN
void S_DATA_Display(void)
{
  uint8_t start_buffer[64U];
  uint8_t i;

  /* Read appli first 64 data bytes */
  if (OBK_Flash_ReadEncrypted(DATA_PRIMARY_OFFSET, start_buffer, sizeof(start_buffer)) != ARM_DRIVER_OK)
  {
    printf("\r\n Error reading data bytes\r\n");
    return;
  }

  /* display the first 32 data bytes which starts from index 32 */
  printf("  -- S Data from OBKey: ");
  for (i = 32U; i < 64U; i++)
  {
    printf("%02x",start_buffer[i]);
  }
  printf("\r\n");

}
#endif /* #ifdef DATA_IMAGE_EN */
