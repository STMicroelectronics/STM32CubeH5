/*
    Copyright (C) 2015-2025, Navaro, All Rights Reserved
    SPDX-License-Identifier: MIT

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "qoraal/qoraal.h"
#include "qoraal-flash/qoraal.h"
#include "qoraal-flash/registry.h"
#include "qoraal-flash/syslog.h"


const QORAAL_FLASH_CFG_T *_qoraal_flash_instance = 0 ;

int32_t
qoraal_flash_instance_init (const QORAAL_FLASH_CFG_T * instance)
{
    if (!_qoraal_flash_instance) {
        _qoraal_flash_instance = instance;


    }
    return EOK;
}

int32_t
qoraal_flash_init_default (const QORAAL_FLASH_CFG_T * instance, 
                            NVOL3_INSTANCE_T * registry,
                            SYSLOG_INSTANCE_T * syslog)
{
    int32_t res = qoraal_flash_instance_init (instance) ;
    if (res != EOK) {
        return res ;


    }

    registry_init (registry) ;
    syslog_init (syslog) ;


    return EOK;
}

int32_t
qoraal_flash_start_default (void)
{
    registry_start () ;
    syslog_start () ;    

    return EOK;
}

int32_t
qoraal_flash_stop_default (void)
{
    registry_stop () ;
    syslog_stop () ;    

    return EOK;
}