/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

/*
 * Original code taken from mcuboot project at:
 * https://github.com/JuulLabs-OSS/mcuboot
 * Git SHA of the original version: ac55554059147fff718015be9f4bd3108123f50a
 * Modifications are Copyright (c) 2019 Arm Limited.
 */

#include <bootutil/sign_key.h>
#include "mcuboot_config/mcuboot_config.h"
#include "platform/include/tfm_attest_hal.h"
#include "platform/include/tfm_plat_crypto_keys.h"
#include "psa/crypto_types.h"
#include "psa/crypto_values.h"
#include "flash_layout.h"
#include "config-boot.h"
#include "low_level_obkeys.h"

#if defined(MCUBOOT_SIGN_RSA)
#if MCUBOOT_SIGN_RSA_LEN == 2048
const unsigned int rsa2048_pub_key_len  = 270;
#if (MCUBOOT_APP_IMAGE_NUMBER == 2) || (MCUBOOT_NS_DATA_IMAGE_NUMBER == 1)
const unsigned int rsa2048_pub_key_len_1  = 270;
#endif
const struct bootutil_key bootutil_keys[] = {
    {
        .key = OBK_Hdpl1_Cfg.Hdpl3SecureAuthenticationPubKey,
        .len = &rsa2048_pub_key_len,
    },
#if (MCUBOOT_APP_IMAGE_NUMBER == 2)
    {
        .key = OBK_Hdpl1_Cfg.Hdpl3NonSecureAuthenticationPubKey,
        .len = &rsa2048_pub_key_len_1,
    },
#endif
#if (MCUBOOT_S_DATA_IMAGE_NUMBER == 1)
    {
        .key = OBK_Hdpl1_Cfg.Hdpl3SecureAuthenticationPubKey,
        .len = &rsa2048_pub_key_len,
    },
#endif
#if (MCUBOOT_NS_DATA_IMAGE_NUMBER == 1)
    {
        .key = OBK_Hdpl1_Cfg.Hdpl3NonSecureAuthenticationPubKey,
        .len = &rsa2048_pub_key_len_1,
    },
#endif
};
#else
const unsigned int rsa3072_pub_key_len  = 398;
#if (MCUBOOT_APP_IMAGE_NUMBER == 2) || (MCUBOOT_NS_DATA_IMAGE_NUMBER == 1)
const unsigned int rsa3072_pub_key_len_1 = 398;
#endif
const struct bootutil_key bootutil_keys[] = {
    {
        .key = OBK_Hdpl1_Cfg.Hdpl3SecureAuthenticationPubKey,
        .len = &rsa3072_pub_key_len,
    },
#if (MCUBOOT_APP_IMAGE_NUMBER == 2)
    {
        .key = OBK_Hdpl1_Cfg.Hdpl3NonSecureAuthenticationPubKey,
        .len = &rsa3072_pub_key_len_1,
    },
#endif
#if (MCUBOOT_S_DATA_IMAGE_NUMBER == 1)
    {
        .key = OBK_Hdpl1_Cfg.Hdpl3SecureAuthenticationPubKey,
        .len = &rsa3072_pub_key_len,
    },
#endif
#if (MCUBOOT_NS_DATA_IMAGE_NUMBER == 1)
    {
        .key = OBK_Hdpl1_Cfg.Hdpl3NonSecureAuthenticationPubKey,
        .len = &rsa3072_pub_key_len_1,
    },
#endif
};
#endif /* MCUBOOT_SIGN_RSA_LEN == 2048 */
#elif defined(MCUBOOT_SIGN_EC256)
const unsigned int ecdsa_pub_key_len = 91;
#if (MCUBOOT_APP_IMAGE_NUMBER == 2) || (MCUBOOT_NS_DATA_IMAGE_NUMBER == 1)
const unsigned int ecdsa_pub_key_len_1 = 91;
#endif
const struct bootutil_key bootutil_keys[] = {
    {
        .key = OBK_Hdpl1_Cfg.Hdpl3SecureAuthenticationPubKey,
        .len = &ecdsa_pub_key_len,
    },
#if (MCUBOOT_APP_IMAGE_NUMBER == 2)
    {
        .key = OBK_Hdpl1_Cfg.Hdpl3NonSecureAuthenticationPubKey,
        .len = &ecdsa_pub_key_len_1,
    },
#endif
#if (MCUBOOT_S_DATA_IMAGE_NUMBER == 1)
    {
        .key = OBK_Hdpl1_Cfg.Hdpl3SecureAuthenticationPubKey,
        .len = &ecdsa_pub_key_len,
    },
#endif
#if (MCUBOOT_NS_DATA_IMAGE_NUMBER == 1)
    {
        .key = OBK_Hdpl1_Cfg.Hdpl3NonSecureAuthenticationPubKey,
        .len = &ecdsa_pub_key_len_1,
    },
#endif
};
#else
#error "No public key available for given signing algorithm."
#endif
const int bootutil_key_cnt = MCUBOOT_IMAGE_NUMBER;
#if defined(MCUBOOT_ENC_IMAGES)
#if defined(MCUBOOT_ENCRYPT_RSA)
const unsigned int enc_rsa_priv_key_len = 1192;
const struct bootutil_key bootutil_enc_key = {
    .key = OBK_Hdpl1_Cfg.Hdpl3EncryptionPrivKey,
    .len = &enc_rsa_priv_key_len,
};
#elif defined(MCUBOOT_ENCRYPT_EC256)

const unsigned int enc_ec256_priv_key_len = 70;
const struct bootutil_key bootutil_enc_key = {
    .key = OBK_Hdpl1_Cfg.Hdpl3EncryptionPrivKey,
    .len = &enc_ec256_priv_key_len,
};
#endif /* MCUBOOT_ENCRYPT_RSA */
#else
const struct bootutil_key bootutil_enc_key = {
    .key = (const unsigned char *)0,
    .len = (const unsigned int *)0,
};
#endif/* MCUBOOT_ENC_IMAGES */
