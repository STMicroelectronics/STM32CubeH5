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

#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "qoraal/qoraal.h"

#define DEBUG_MSG_SIZE          128

int
debug_printf (const char *format, ...)
{
    va_list         args;
    va_start (args, format) ;
    char debug_msg[DEBUG_MSG_SIZE] ;
    int32_t count = vsnprintf ((char*)debug_msg, DEBUG_MSG_SIZE, format, args) ;
    va_end (args) ;
    debug_msg [DEBUG_MSG_SIZE-1] = '\0';
    qoraal_debug_print (debug_msg) ;
    return count ;
}

void
debug_assert(const char *format, ...)
{
    va_list         args;
    va_start (args, format) ;
    char debug_msg[DEBUG_MSG_SIZE] ;
    vsnprintf ((char*)debug_msg, DEBUG_MSG_SIZE, format, args) ;
    va_end (args) ;
    debug_msg [DEBUG_MSG_SIZE-1] = '\0';
    qoraal_debug_assert (debug_msg) ;
}

