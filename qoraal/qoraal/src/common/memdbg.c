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
                                        INCLUDE FILES
==================================================================================================*/
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "qoraal/common/memdbg.h"

/*=================================================================================================
                                        DEFINES
==================================================================================================*/
/*==================================================================================================
                                     LOCAL VARIABLES
==================================================================================================*/

/*==================================================================================================
FUNCTION: get_printable          
DESCRIPTION: 
    Either the char is return or if the char is not printable the defined replacement are returned.
   
ARGUMENTS PASSED:
RETURN VALUE:
PRE-CONDITIONS:
POST-CONDITIONS:
IMPORTANT NOTES:
==================================================================================================*/
char
get_printable (char c) 
{
    //return isprint(c) ? c : CHAR_NONPRINTABLE ;;
    if ((c >= 0x20) && (c<= 0x7E)) return c ;
    return CHAR_NONPRINTABLE ;
}

/*==================================================================================================
FUNCTION: dbg_format_mem_dump_buffer          
DESCRIPTION: 
   returns the user supplied buffer where the result was printed to.

ARGUMENTS PASSED:
RETURN VALUE:
PRE-CONDITIONS:
POST-CONDITIONS:
IMPORTANT NOTES:
==================================================================================================*/
char* dbg_format_mem_dump_buffer (char* buffer, int len, const char* data, int size, unsigned int print_addr)
{
    return dbg_format_mem_dump_width (buffer, len, data, size, print_addr, DUMP_DATA_LINE_SIZE) ;
}

/*==================================================================================================
FUNCTION: dbg_mem_dump          
DESCRIPTION: 
   returns the user supplied buffer where the result was printed to. The number of bytes per line
   are specified in the width parameter.

ARGUMENTS PASSED:
RETURN VALUE:
PRE-CONDITIONS:
POST-CONDITIONS:
IMPORTANT NOTES:
==================================================================================================*/
char* dbg_format_mem_dump_width (char* buffer, int buffer_len, const char* data, int size, unsigned int print_addr, int width)
{
    int i, j ;
    int len = 0;
    int left ;

#define LINE_LENGTH(w)      (18 + w * 3 + 5 + w + 2)

    if (buffer_len < 22 ) {
        buffer[0] = '\0' ;
        return buffer ;
    }

//  len += sprintf (&buffer[len], "dumping %.4d bytes%s", size, MEMDBG_NEWLINE);

    for (j=0; ((j+1)*width) < size && (len<buffer_len-LINE_LENGTH(width)) ; j++) {
#ifndef MEMDBG_MAKE_CARRAY
        len += sprintf (&buffer[len], "    0x%.8x:   ", print_addr + j*width /*&data[j*width]*/);
#endif
        for (i=0; i<width; i++) {
            len += sprintf (&buffer[len], "%s%.2X", DUMP_HEX_SEPERATOR_LEAD, (char)data[i+j*width]&0xFF);
            len += sprintf (&buffer[len], "%s", DUMP_HEX_SEPERATOR_TRAIL);
        }
#ifndef MEMDBG_MAKE_CARRAY
        len += sprintf (&buffer[len], "%s", ASCII_SEPERATOR) ;
        for (i=0;  i<width; i++) {
            len += sprintf (&buffer[len], "%c", get_printable(data[i+j*width]));
        }
#endif
        len += sprintf (&buffer[len], "%s", MEMDBG_NEWLINE);
    }


    if (len<(buffer_len-LINE_LENGTH(width))) {
        left = size-j*width ;
#ifndef MEMDBG_MAKE_CARRAY
        len += sprintf (&buffer[len], "    0x%.8x:   ", print_addr + j*width /*&data[j*width]*/);
#endif

        for (i=0; i<width; i++) {
            if (i < left) {
                len += sprintf (&buffer[len], "%s%.2X", DUMP_HEX_SEPERATOR_LEAD, (char)data[i+j*width]&0xFF);
                len += sprintf (&buffer[len], "%s", DUMP_HEX_SEPERATOR_TRAIL);
            } else {
#ifndef MEMDBG_MAKE_CARRAY
                len += sprintf (&buffer[len], "%s  %s" , DUMP_HEX_SEPERATOR_LEAD,  DUMP_HEX_SEPERATOR_TRAIL);
#endif
            }
        }
#ifndef MEMDBG_MAKE_CARRAY
        len += sprintf (&buffer[len], "%s", ASCII_SEPERATOR) ;
        for (i=0; i<left; i++) {
            len += sprintf (&buffer[len], "%c",  get_printable(data[i+j*width]));
        }
#endif
        len += sprintf (&buffer[len], "%s", MEMDBG_NEWLINE);
    }

    return buffer ;
}

/*==================================================================================================
FUNCTION: dbg_format_mem_dump_line
DESCRIPTION:
   returns the user supplied buffer where the result was printed to. The number of bytes per line
   are specified in the width parameter.

ARGUMENTS PASSED:
RETURN VALUE:
PRE-CONDITIONS:
POST-CONDITIONS:
IMPORTANT NOTES:
==================================================================================================*/
char* dbg_format_mem_dump_line (char* buffer, int buffer_len, const char* data, int size)
{
    int i  ;
    int len = 0;

    for (i=0; i<size; i++) {
        len += sprintf (&buffer[len], "%.2X ", (char)data[i]&0xFF);
        if (len >= buffer_len-4) break ;
    }

    return buffer ;
}

char* dbg_format_mem_2html_buffer (char* buffer, int buffer_len, const char* data, int size, unsigned int print_addr)
{
#undef LINE_LENGTH
#define LINE_LENGTH(w)      (18 + w * 3 + 5 + w + 1)


    int j ;
    int len = 0 ;
    if (buffer_len < 22 ) {
        buffer[0] = '\0' ;
        return buffer ;
    }

//  len += sprintf (&buffer[len], "dumping %.4d bytes%s", size, MEMDBG_NEWLINE);
    len += sprintf (&buffer[len], "<b>%d bytes from 0x%.8x:</b><br>", size, print_addr );

    for (j=0; j< size && len < buffer_len - 8 ; j++) {
        switch(data[j]) {
        case '\r':  break ;
        case '\n': strcpy(&buffer[len], "<br>\r\n"); len += 6;  break ;

        default: buffer[len++] = data[j] ; break ;
        }
    }

    buffer[len] = '\0' ;
    return buffer ;
}

