#if !defined __MEMDBG_H__
#define __MEMDBG_H__
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

/*==================================================================================================

    Header Name: memdbg.h

    General Description:    Just routines to print out memory nicely formatted 
                            in HEX with the ASCII on the right side. Example:
  
                                0x0012f378:   01 00 5e 7f ff fa 00 ff 44 f6 48 89   -  ..^.....D.H.
                                0x0012f384:   08 00 45 00 00 a1 db b5 00 00 01 11   -  ..E.........
                                0x0012f390:   2c f3 c0 a8 00 01 ef ff ff fa 0f 28   -  ,=.........(
                                0x0012f39c:   07 6c 00 8d 91 b2 4d 2d 53 45 41 52   -  .l....M-SEAR
                                0x0012f3a8:   43 48 20 2a 20 48 54 54 50 2f 31 2e   -  CH * HTTP/1.
                                0x0012f3b4:   31 0d 0a 48 6f 73 74 3a 32 33 39 2e   -  1..Host:239.
                                0x0012f3c0:   32 35 35 2e 32 35 35 2e 32 35 30 3a   -  255.255.250:
                                0x0012f3cc:   31 39 30 30 0d 0a 53 54 3a 75 72 6e   -  1900..ST:urn
                                0x0012f3d8:   3a 73 63 68 65 6d 61 73 2d 75 70 6e   -  :schemas-upn
                                0x0012f3e4:   70 2d 6f 72 67 3a 64 65 76 69 63 65   -  p-org:device
                                0x0012f3f0:   3a 49 6e 74 65 72 6e 65 74 47 61 74   -  :InternetGat
                                0x0012f3fc:   65 77 61 79 44 65 76 69 63 65 3a 31   -  ewayDevice:1
                                0x0012f408:   0d 0a 4d 61 6e 3a 22 73 73 64 70 3a   -  ..Man:"ssdp:
                                0x0012f414:   64 69 73 63 6f 76 65 72 22 0d 0a 4d   -  discover"..M
                                0x0012f420:   58 3a 33 0d 0a 0d 0a                  -  X:3....


==================================================================================================*/


/*==================================================================================================
                                           INCLUDES
==================================================================================================*/
#include <stdio.h>
#include <stdarg.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/*==================================================================================================
                                           CONSTANTS
==================================================================================================*/
/*==================================================================================================
                                             ENUMS
==================================================================================================*/
/*==================================================================================================
                                 STRUCTURES AND OTHER TYPEDEFS
==================================================================================================*/
/*==================================================================================================
                                 GLOBAL VARIABLE DECLARATIONS
==================================================================================================*/
/*=================================================================================================
                                        DEFINES
==================================================================================================*/
#ifndef MEMDBG_MAKE_CARRAY
#define CHAR_NONPRINTABLE           '.'
#define DBG_BUFFER_SIZE             1024
#define DUMP_DATA_LINE_SIZE         16
#define DUMP_HEX_SEPERATOR_TRAIL    " "
#define DUMP_HEX_SEPERATOR_LEAD     ""
#define ASCII_SEPERATOR             " - "
#define MEMDBG_NEWLINE              "\r\n"
#else
#define CHAR_NONPRINTABLE           '.'
#define DBG_BUFFER_SIZE             1024
#define DUMP_DATA_LINE_SIZE         8
#define DUMP_HEX_SEPERATOR_TRAIL    ", "
#define DUMP_HEX_SEPERATOR_LEAD     "0x"
#define ASCII_SEPERATOR             " // "
#define MEMDBG_NEWLINE              "\r\n"
#endif
/*==================================================================================================
                                            MACROS
==================================================================================================*/

/*==================================================================================================
                                     FUNCTION PROTOTYPES
==================================================================================================*/

#if 1 // defined DEBUG /* || defined _DEBUG */
char* dbg_format_mem_dump_buffer (char* buffer, int len, const char* data, int size, unsigned int print_addr) ;
char* dbg_format_mem_dump_width (char* buffer, int len, const char* data, int size, unsigned int print_addr, int width) ;
char* dbg_format_mem_2html_buffer (char* buffer, int buffer_len, const char* data, int size, unsigned int print_addr) ;

#endif /* DEBUG */

#ifdef __cplusplus
}
#endif

#endif /* __DBG_H__ */

