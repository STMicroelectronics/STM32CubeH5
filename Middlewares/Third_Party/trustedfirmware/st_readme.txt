
  @verbatim
  ******************************************************************************
  *
  *         COPYRIGHT 2021 STMicroelectronics, All Rights Reserved
  *
  * @file    st_readme.txt
  * @author  MCD Application Team
  * @brief   This file lists the main modification done by STMicroelectronics on
  *          trustedfirmware for integration with STM32Cube solution.
  ******************************************************************************
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  ******************************************************************************
  @endverbatim

### 24-April-2023 ###
========================
    + Fix Keil compile warnings

### 26-January-2023 ###
========================
    + Fix GCC compile warnings
    + Add tools to verify EAT token (st_tools folder)

### 03-August-2022 ###
========================
    + FOSS Audit Neutral license
    + Starting from version TF-Mv1.3.0 from https://git.trustedfirmware.org/TF-M/trusted-firmware-m.git/refs/
      only BL2 source files necessary to OEM-iROT applications have been kept.
      TFM application is not part of STM32Cube FW package. Thus, other files are not required.
    + remove compilation warning

### 14-February-2022 ###
========================
    + remove TFM services and keep only BL2 source files necessary to SBSFU application
    + remove compilation warning

### 11-October-2021 ###
========================
    + use tfm 1.3.0 release
    + add support of firmware update partition
    + add support of data provisionning
