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



#ifndef __WABOUT_H__
#define __WABOUT_H__


#include <stdint.h>
#include "qoraal-http/httpserver.h"


/*===========================================================================*/
/* Client constants.                                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Client pre-compile time settings.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* Client data structures and types.                                         */
/*===========================================================================*/

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif


int32_t         wabout_handler (HTTP_USER_T *user, uint32_t method, char* endpoint) ;
int32_t         wserver_handler_about2 (HTTP_USER_T *user, uint32_t method, char* endpoint) ;
int32_t         wserver_handler_about3 (HTTP_USER_T *user, uint32_t method, char* endpoint) ;
const char*     wabout_ctrl (HTTP_USER_T *user, uint32_t method, char* endpoint, uint32_t type) ;

#ifdef __cplusplus
}
#endif

#endif /* __WABOUT_H__ */

