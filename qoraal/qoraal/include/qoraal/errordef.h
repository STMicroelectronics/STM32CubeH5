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

#ifndef __ERRORDEF_H__
#define __ERRORDEF_H__

#define QORAAL_ERROR             -2300

#define EOK                     0
#define EFAIL                   -1

#ifndef E_TIMEOUT
#define E_TIMEOUT               (QORAAL_ERROR-2)
#endif

#ifndef E_INVALID
#define E_INVALID               (QORAAL_ERROR-3)
#endif

#ifndef E_NOMEM
#define E_NOMEM                 (QORAAL_ERROR-4)
#endif

#ifndef E_PARM
#define E_PARM                  (QORAAL_ERROR-5)
#endif

#ifndef E_BUSY
#define E_BUSY                  (QORAAL_ERROR-6)
#endif

#ifndef E_NOIMPL
#define E_NOIMPL                (QORAAL_ERROR-7)
#endif

#ifndef E_NOTFOUND
#define E_NOTFOUND              (QORAAL_ERROR-8)
#endif

#ifndef E_SRCH
#define E_SRCH                  (QORAAL_ERROR-9)
#endif

#ifndef E_UNEXP
#define E_UNEXP                 (QORAAL_ERROR-10)
#endif

#ifndef E_NOTRDY
#define E_NOTRDY                (QORAAL_ERROR-11)
#endif

#ifndef E_VALIDATION
#define E_VALIDATION            (QORAAL_ERROR-16)
#endif

#ifndef E_INVAL
#define E_INVAL                 (QORAAL_ERROR-17)
#endif

#ifndef E_BOF
#define E_BOF                   (QORAAL_ERROR-18)
#endif

#ifndef E_EOF
#define E_EOF                   (QORAAL_ERROR-19)
#endif

#ifndef E_EMPTY
#define E_EMPTY                 (QORAAL_ERROR-20)
#endif

#ifndef E_VERSION
#define E_VERSION               (QORAAL_ERROR-21)
#endif

#ifndef E_CORRUPT
#define E_CORRUPT               (QORAAL_ERROR-22)
#endif

#ifndef E_CANCELED
#define E_CANCELED              (QORAAL_ERROR-23)
#endif

#ifndef E_FILE
#define E_FILE                  (QORAAL_ERROR-24)
#endif

#ifndef E_FULL
#define E_FULL                  (QORAAL_ERROR-25)
#endif

#ifndef E_CONNECTION
#define E_CONNECTION            (QORAAL_ERROR-25)
#endif

#ifndef E_ENTRYNOTFOUND
#define E_ENTRYNOTFOUND         (QORAAL_ERROR-26)
#endif

#ifndef E_AUTH
#define E_AUTH                  (QORAAL_ERROR-27)
#endif

#ifndef E_ABORT
#define E_ABORT                  (QORAAL_ERROR-28)
#endif

#ifndef E_SYS
#define E_SYS                  (QORAAL_ERROR-29)
#endif

#ifndef E_RONLY
#define E_RONLY                (QORAAL_ERROR-30)
#endif

#ifndef E_UNKNOWN
#define E_UNKNOWN               (QORAAL_ERROR-63)
#endif


#endif /* __ERRORDEF_H__ */

/*@}*/
