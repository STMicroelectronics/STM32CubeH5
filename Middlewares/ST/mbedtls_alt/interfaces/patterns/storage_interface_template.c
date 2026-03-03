/**
  ******************************************************************************
  * @file    storage_interface_template.c
  * @author  MCD Application Team
  * @brief   Implementation of storage interface template
  *          This file should be copied to the application folder and renamed
  *          to storage_interface.c
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "storage_interface.h"
#include "string.h"
#include <stdio.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Functions Definition ------------------------------------------------------*/

/**
  * @brief  A function that store the object in storage.
  * @param  obj_uid : unique identifier used for identifying the object.
  * @param  obj_length : size of the object in bytes.
  * @param  p_obj : a pointer to the object to be stored.
  * @retval PSA_SUCCESS if success, an error code otherwise.
  */
psa_status_t storage_set(uint64_t obj_uid,
                         uint32_t obj_length,
                         const void *p_obj)
{
  psa_status_t status = PSA_ERROR_STORAGE_FAILURE;
  /* User code start */

  /* User code end */

  return status;
}

/**
  * @brief  A function that retrieve the object from storage.
  * @param  obj_uid : unique identifier used for identifying the object.
  * @param  obj_offset : The starting offset of the object.
  * @param  obj_length : size of the object in bytes.
  * @param  p_obj : a pointer to the object to be retrieved.
  * @retval PSA_SUCCESS if success, an error code otherwise.
  */
psa_status_t storage_get(uint64_t obj_uid,
                         uint32_t obj_offset,
                         uint32_t obj_length,
                         void *p_obj)
{
  psa_status_t status = PSA_ERROR_DOES_NOT_EXIST;
  /* User code start */

  /* User code end */

  return status;
}

/**
  * @brief  A function that retrieve the object info using the object unique
  *         identifier.
  * @param  obj_uid : unique identifier used for identifying data.
  * @param  p_obj_info : a pointer to metadata: ID, size, flags.
  * @param  obj_info_size : size of the object info.
  * @retval PSA_SUCCESS if success, an error code otherwise.
  */
psa_status_t storage_get_info(uint64_t obj_uid, void *p_obj_info, uint32_t obj_info_size)
{
  psa_status_t status = PSA_ERROR_DOES_NOT_EXIST;
  /* User code start */

  /* User code end */

  return status;
}

/**
  * @brief  A function that remove the object from storage.
  * @param  obj_uid : unique identifier used for identifying the object.
  * @param  obj_size : size of object to be removed from storage.
  * @retval PSA_SUCCESS if success, an error code otherwise.
  */
psa_status_t storage_remove(uint64_t obj_uid, uint32_t obj_size)
{
  psa_status_t status = PSA_ERROR_STORAGE_FAILURE;

  /* User code start */

  /* User code end */

  return status;
}
