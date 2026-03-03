/**
  ******************************************************************************
  * @file    psa_its_alt.c
  * @author  MCD Application Team
  * @brief   Implementation of PSA Internal Trusted Storage alternative
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
#include "psa_crypto_storage.h"
#if defined(PSA_USE_ITS_ALT)
#include "psa_its_alt.h"

/* Global variables ----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/*  ITS encryption key ID */
#if defined(PSA_USE_ENCRYPTED_ITS)
static mbedtls_svc_key_id_t its_key = {0};
#endif /* PSA_USE_ENCRYPTED_ITS */

/* Private function prototypes -----------------------------------------------*/
#if defined(PSA_USE_ENCRYPTED_ITS)
static psa_status_t its_crypto_setkey(void);
static psa_status_t its_encrypt_obj(const void *p_data, uint32_t data_length, its_obj_t *p_its_obj);
static psa_status_t its_decrypt_obj(its_obj_t *p_its_obj, const uint32_t data_length, void *p_data);
#endif /* PSA_USE_ENCRYPTED_ITS */

/* Functions Definition ------------------------------------------------------*/
#if defined(PSA_USE_ENCRYPTED_ITS)
/**
  * @brief  A function that set ITS encryption key based key derivation operation.
  * @note   ITS_ENCRYPTION_SECRET_KEY_ID identifier should be used by user
  *         application to import ITS encryption key.
  * @retval PSA_SUCCESS if success, an error code otherwise.
  */
static psa_status_t its_crypto_setkey(void)
{
  psa_status_t status;
  psa_key_attributes_t attributes = PSA_KEY_ATTRIBUTES_INIT;
#if !defined(USE_HUK)
  psa_key_derivation_operation_t op = PSA_KEY_DERIVATION_OPERATION_INIT;
  uint8_t its_key_label[] = "storage_key";
#endif /* ! USE_HUK */

  /* Set the key attributes for the storage encryption secret key */
  psa_set_key_usage_flags(&attributes, (PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT));
  psa_set_key_algorithm(&attributes, PSA_ALG_AEAD_WITH_SHORTENED_TAG(PSA_ALG_GCM, ITS_TAG_SIZE));
  psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
#if defined(USE_HUK)
  psa_set_key_bits(&attributes, 0U);
  status = psa_import_key(&attributes, NULL, 0U, &its_key);
  return status;
#else
  psa_set_key_bits(&attributes, PSA_BYTES_TO_BITS(32U));
  /* Set up a key derivation operation based on hash function */
  status = psa_key_derivation_setup(&op, PSA_ALG_HKDF(PSA_ALG_SHA_256));
  if (status != PSA_SUCCESS)
  {
    return status;
  }

  /* Set up a key derivation operation using ITS encryption secret key,
     the secret key should be imported by the application using PSA key import
     and ITS_ENCRYPTION_SECRET_KEY_ID.
     storage interface should write the key in protected location
     */
  status = psa_key_derivation_input_key(&op, PSA_KEY_DERIVATION_INPUT_SECRET,
                                        ITS_ENCRYPTION_SECRET_KEY_ID);
  if (status != PSA_SUCCESS)
  {
    goto err_release_op;
  }

  /* Supply the ITS key label as an input to the key derivation */
  status = psa_key_derivation_input_bytes(&op, PSA_KEY_DERIVATION_INPUT_INFO,
                                          its_key_label,
                                          sizeof(its_key_label));
  if (status != PSA_SUCCESS)
  {
    goto err_release_op;
  }

  /* Create the storage key from the key derivation operation */
  status = psa_key_derivation_output_key(&attributes, &op, &its_key);
  if (status != PSA_SUCCESS)
  {
    goto err_release_op;
  }

  /* Free resources associated with the key derivation operation */
  status = psa_key_derivation_abort(&op);
  if (status != PSA_SUCCESS)
  {
    goto err_release_key;
  }

  return PSA_SUCCESS;

err_release_key:
  (void)psa_destroy_key(its_key);

err_release_op:
  (void)psa_key_derivation_abort(&op);

  return PSA_ERROR_GENERIC_ERROR;
#endif /* USE_HUK */
}

/**
  * @brief  A function that encrypt an object using ITS encryption key imported by user
  * @note   ITS_ENCRYPTION_SECRET_KEY_ID identifier should be used by user
  *         application to import ITS encryption key.
  * @param  p_data : a pointer to the data to be encrypted.
  * @param  data_length : size of the data in bytes.
  * @param  p_its_obj :  a pointer to the object where the encrypted data will be placed.
  * @retval PSA_SUCCESS if success, an error code otherwise.
  */
static psa_status_t its_encrypt_obj(const void *p_data, uint32_t data_length, its_obj_t *p_its_obj)
{
  psa_status_t status = PSA_ERROR_STORAGE_FAILURE;

  /* Set ITS encryption key using ITS_ENCRYPTION_SECRET_KEY_ID
   *  and Key derivation algorithme
   */
  status = its_crypto_setkey();
  if (status != PSA_SUCCESS)
  {
    return status;
  }

  /* Generate IVs for AES GCM encryption
   * IVs are stored with encrypted object
   */
  status = psa_generate_random(p_its_obj->obj_iv, ITS_IV_SIZE);
  if (status != PSA_SUCCESS)
  {
    return status;
  }

  /* Encrypt object using ITS encryption key defined by ITS_ENCRYPTION_SECRET_KEY_ID */
  status = psa_aead_encrypt(its_key, PSA_ALG_GCM,
                            p_its_obj->obj_iv, ITS_IV_SIZE,
                            p_its_obj->obj_info.obj_id, sizeof(p_its_obj->obj_info.obj_id),
                            p_data, data_length,
                            p_its_obj->obj_tag,
                            sizeof(p_its_obj->obj) + sizeof(p_its_obj->obj_tag), p_its_obj->obj_info.size);

  if (status != PSA_SUCCESS)
  {
    return PSA_ERROR_GENERIC_ERROR;
  }

  /* Increase object size by adding IVs size */
  *p_its_obj->obj_info.size = *p_its_obj->obj_info.size + ITS_IV_SIZE;

  return status;
}

/**
  * @brief  A function that decrypt an object using ITS encryption key imported by user
  * @note   ITS_ENCRYPTION_SECRET_KEY_ID identifier should be used by user
  *         application to import ITS encryption key.
  * @param  p_its_obj :  a pointer to the object to be decrypted.
  * @param  data_length : size of the requested data in bytes.
  * @param  p_data : a pointer to the buffer where the decrypted data will be placed.
  * @retval PSA_SUCCESS if success, an error code otherwise.
  */
static psa_status_t its_decrypt_obj(its_obj_t *p_its_obj, const uint32_t data_length, void *p_data)
{
  psa_status_t status = PSA_ERROR_STORAGE_FAILURE;

  /* Set ITS encryption key using ITS_ENCRYPTION_SECRET_KEY_ID
   *  and Key derivation algorithme
   */
  status = its_crypto_setkey();
  if (status != PSA_SUCCESS)
  {
    return status;
  }

  /* Decrypt object using ITS encryption key defined by ITS_ENCRYPTION_SECRET_KEY_ID */
  status = psa_aead_decrypt(its_key, PSA_ALG_GCM,
                            p_its_obj->obj_iv, ITS_IV_SIZE,
                            p_its_obj->obj_info.obj_id, sizeof(p_its_obj->obj_info.obj_id),
                            p_its_obj->obj_tag, (*p_its_obj->obj_info.size - sizeof(p_its_obj->obj_iv)),
                            p_data, data_length,
                            p_its_obj->obj_info.size);

  if (status != PSA_SUCCESS)
  {
    return PSA_ERROR_INVALID_SIGNATURE;
  }

  if (*p_its_obj->obj_info.size != data_length)
  {
    return PSA_ERROR_CORRUPTION_DETECTED;
  }

  if (status != PSA_SUCCESS)
  {
    return PSA_ERROR_GENERIC_ERROR;
  }

  return status;
}
#endif /* PSA_USE_ENCRYPTED_ITS */

/**
  * @brief  A function that store data in secure storage, data is encrypted
  *         if PSA_USE_ENCRYPTED_ITS is enabled.
  * @note   The data is stored as an object using storgae interface.
  * @param  uid : unique identifier used for identifying data.
  * @param  data_length : size of the data in bytes.
  * @param  p_data : a pointer to the data to be stored.
  * @param  create_flags : data flag indicate data state Set = 1, Reset = 0.
  * @retval PSA_SUCCESS if success, an error code otherwise.
  */
psa_status_t psa_its_set(psa_storage_uid_t uid,
                         uint32_t data_length,
                         const void *p_data,
                         psa_storage_create_flags_t create_flags)
{
  psa_status_t status = PSA_ERROR_STORAGE_FAILURE;
  its_obj_t its_obj = {0};
  uint32_t its_obj_length = 0U;

  if (uid == ITS_INVALID_UID)
  {
    return PSA_ERROR_INVALID_HANDLE;
  }

  if (data_length != 0)
  {
    its_obj_length = sizeof(its_obj_info_t);

    /* Fill object with data infomations */
    memcpy(its_obj.obj_info.obj_id, &uid, sizeof(psa_storage_uid_t));
    memcpy(its_obj.obj_info.size, &data_length, sizeof(data_length));
    memcpy(its_obj.obj_info.flags, &create_flags, sizeof(create_flags));

#if defined(PSA_USE_ENCRYPTED_ITS)
    if (uid != ITS_ENCRYPTION_SECRET_KEY_ID) /* Don't encrypt the ITS encryption key */
    {
      /* Encrypt data and fill the object */
      status = its_encrypt_obj(p_data, data_length, &its_obj);
      /* Increase object size by adding size of encrypted data  */
      its_obj_length = its_obj_length + *its_obj.obj_info.size;
    }
    else
#endif /* PSA_USE_ENCRYPTED_ITS */
    {
      /* Fill object with data */
      memcpy(its_obj.obj_iv, p_data, data_length);
      /* Increase object size by adding size of data  */
      its_obj_length = its_obj_length + data_length;
      status = PSA_SUCCESS;
    }

    if (status != PSA_SUCCESS)
    {
      return PSA_ERROR_STORAGE_FAILURE;
    }
    /* Depending to key ID, the storage interface should save
     * ITS encryption key in secure locations different to ITS keys location
     */
    status = storage_set(uid, its_obj_length, &its_obj);
    if (status != PSA_SUCCESS)
    {
      return PSA_ERROR_STORAGE_FAILURE;
    }
  }

  memset(&its_obj, 0, sizeof(its_obj));

  return status;
}

/**
  * @brief  A function that retrieve data from secure storage, data is decrypted
  *         if PSA_USE_ENCRYPTED_ITS is enabled.
  * @note   The data is loaded as an object using storgae interface.
  * @param  uid : unique identifier used for identifying data.
  * @param  data_offset : The starting offset of the data requested
  * @param  data_length : size of the data in bytes.
  * @param  p_data : a pointer to the data to be retrieved.
  * @param  p_data_length : a pointer to the actual size of stored data.
  * @retval PSA_SUCCESS if success, an error code otherwise.
  */
psa_status_t psa_its_get(psa_storage_uid_t uid,
                         uint32_t data_offset,
                         uint32_t data_length,
                         void *p_data,
                         size_t *p_data_length)
{
  psa_status_t status = PSA_ERROR_STORAGE_FAILURE;
  struct psa_storage_info_t info;
  its_obj_t its_obj = {0};
  uint32_t its_obj_length = 0U;

  if (uid == ITS_INVALID_UID)
  {
    return PSA_ERROR_INVALID_HANDLE;
  }

  /* Get data ID size and validity  */
  status = psa_its_get_info(uid, &info);
  if (status != PSA_SUCCESS)
  {
    return PSA_ERROR_STORAGE_FAILURE;
  }

  if ((info.size == 0) || (info.size != data_length))
  {
    return PSA_ERROR_DATA_CORRUPT;
  }

  if (p_data_length != NULL)
  {
    *p_data_length = info.size;
  }

  its_obj_length = info.size;

#if defined(PSA_USE_ENCRYPTED_ITS)
  if (uid != ITS_ENCRYPTION_SECRET_KEY_ID)
  {
    /* Encrypted object size includes size of IVs anf tag */
    its_obj_length = its_obj_length + ITS_IV_SIZE + ITS_TAG_SIZE;
    status = storage_get(uid, data_offset, its_obj_length, its_obj.obj_iv);

    /* Fill object with infomations */
    memcpy(its_obj.obj_info.obj_id, &uid, sizeof(psa_storage_uid_t));
    memcpy(its_obj.obj_info.size, &its_obj_length, sizeof(its_obj_length));

    /* Decrypt the object */
    status = its_decrypt_obj(&its_obj, data_length, p_data);

  }
  else
#endif /* PSA_USE_ENCRYPTED_ITS */
  {
    status = storage_get(uid, data_offset, its_obj_length, p_data);
  }

  if (status != PSA_SUCCESS)
  {
    return PSA_ERROR_STORAGE_FAILURE;
  }

  memset(&its_obj, 0, sizeof(its_obj));

  return status;
}

/**
  * @brief  A function that retrieve metadata using the data unique identifier.
  * @param  uid : unique identifier used for identifying data.
  * @param  p_info : a pointer to metadata: ID, size, flags.
  * @retval PSA_SUCCESS if success, an error code otherwise.
  */
psa_status_t psa_its_get_info(psa_storage_uid_t uid,
                              struct psa_storage_info_t *p_info)
{
  its_obj_info_t p_obj_info = {0};
  /* Check data ID validity */
  if (uid == ITS_INVALID_UID)
  {
    return PSA_ERROR_INVALID_HANDLE;
  }

  /* Check the data ID existence in storage */
  if (storage_get_info(uid, &p_obj_info, sizeof(its_obj_info_t)) != PSA_SUCCESS)
  {
    return PSA_ERROR_DOES_NOT_EXIST;
  }

  p_info->size = *(p_obj_info.size);
  p_info->flags = *(uint32_t *)(p_obj_info.flags);

  /* Encrypted key size includes size of IVs anf tag */
#if defined(PSA_USE_ENCRYPTED_ITS)
  if (uid != ITS_ENCRYPTION_SECRET_KEY_ID)
  {
    p_info->size =  p_info->size - (ITS_IV_SIZE + ITS_TAG_SIZE);

  }
#endif /* PSA_USE_ENCRYPTED_ITS */

  return PSA_SUCCESS;
}

/**
  * @brief  A function that remove data from secure storage.
  * @note   Red only data remove is rejectred.
  * @param  uid : unique identifier used for identifying data.
  * @retval PSA_SUCCESS if success, an error code otherwise.
  */
psa_status_t psa_its_remove(psa_storage_uid_t uid)
{
  psa_status_t status = PSA_ERROR_STORAGE_FAILURE;
  struct psa_storage_info_t info;

  /* Get data ID size and validity  */
  status = psa_its_get_info(uid, &info);

  if (status != PSA_SUCCESS)
  {
    return PSA_ERROR_DOES_NOT_EXIST;
  }

  /* Reject red only data ID remove */
  if (info.flags == 1)
  {
    return PSA_ERROR_NOT_PERMITTED;
  }

  /* Remove data ID from the storage */
  status = storage_remove(uid, info.size);

  if (status != PSA_SUCCESS)
  {
    return PSA_ERROR_STORAGE_FAILURE;
  }

  return status;
}

#endif /* PSA_USE_ITS_ALT */
