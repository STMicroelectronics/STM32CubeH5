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


/**
 * @file        rtclib.h
 * @author      Natie van Rooyen <natie@navaro.nl>
 * @date        9 Apr 2015
 * @version
 *
 * @section DESCRIPTION
 *
 *  Real Time Clock API.
 */


#ifndef __RTCLIB_H__
#define __RTCLIB_H__

#include <stdint.h>

#define DBG_MESSAGE_RTC(severity, fmt_str, ...)    DBG_MESSAGE_T_REPORT (SVC_LOGGER_TYPE(severity,0), 0, fmt_str, ##__VA_ARGS__)

/*===========================================================================*/
/* Module data structures and types.                                         */
/*===========================================================================*/

/**
 * @brief   Date structure.
 * @details This structure represents a the date in year, month and day
 *          of month values.
 */
typedef struct RTCLIB_DATE_S {
    union {
        uint32_t        date ;
        struct {
        uint8_t         day ;   /**< @brief Day of month.   */
        uint8_t         month ; /**< @brief Month.   */
        uint16_t        year ;  /**< @brief Year.   */
        } ;
    };
} RTCLIB_DATE_T ;

/**
 * @brief   Time structure.
 * @details This structure represents a the time in hour of day,
 *          minute and seconds.
 */
typedef struct RTCLIB_TIME_S {
    union {
        uint32_t        time ;
        struct {
        uint8_t         second ;    /**< @brief Second.   */
        uint8_t         minute ;    /**< @brief Minute.   */
        uint8_t         hour ;      /**< @brief Hour.   */
        uint8_t         day ;       /**< @brief day of week.   */
        } ;
    };
} RTCLIB_TIME_T ;

/*===========================================================================*/
/* Constants.                                                                */
/*===========================================================================*/


#define RTCLOCK_FIRST_DATE             0x07DC0C16      // December 22nd, 2012
#define RTCLOCK_FIRST_TIME             0x00000000

#define RTCLOCK_LAST_DATE             0x0BB80101      // January 1st, 3000
#define RTCLOCK_LAST_TIME             0x00000000

#define RTCLOCK_RESET_DATE             RTCLOCK_FIRST_DATE      // December 22nd, 2012
#define RTCLOCK_RESET_TIME             RTCLOCK_FIRST_TIME


#define RTCLIB_EPOCH_GDAY               2440588 // (1970*365 + 1970/4 - 1970/100 + 1970/400 + (1*306 + 5)/10 + (1 - 1))
#define RTCLIB_SET_ALARM_MAX            ((604800/2) - 1)  // half week
typedef void (*RTCLIB_ALARM_CALLBACK_T)(void) ;

/*===========================================================================*/
/* External declarations.                                                    */
/*===========================================================================*/

#ifdef __cplusplus
extern "C" {
#endif


RTCLIB_TIME_T   rtc_get_time (void) ;
RTCLIB_DATE_T   rtc_get_date (void) ;

void            rtc_localtime (uint32_t timestamp, RTCLIB_DATE_T* date, RTCLIB_TIME_T* time) ;
uint32_t        rtc_time (void) ;
uint32_t        rtc_mktime (RTCLIB_DATE_T date, RTCLIB_TIME_T time) ;
RTCLIB_DATE_T   rtc_get_gdate(uint32_t jday) ;
int32_t         rtc_get_jday (RTCLIB_DATE_T d) ;
RTCLIB_TIME_T   rtc_time_add (RTCLIB_TIME_T from, uint32_t seconds) ;
int32_t         rtc_date_compare (RTCLIB_DATE_T date1, RTCLIB_DATE_T date2) ;
int32_t         rtc_time_to_expire (RTCLIB_TIME_T time) ;
uint32_t        rtc_seconds_elapsed (RTCLIB_DATE_T date, RTCLIB_TIME_T time) ;
uint32_t        rtc_seconds_diff (RTCLIB_TIME_T time1, RTCLIB_TIME_T time2) ;
int32_t         rtc_days_diff (RTCLIB_DATE_T date1, RTCLIB_DATE_T date2) ;
int32_t         rtc_time_compare (RTCLIB_TIME_T time1, RTCLIB_TIME_T time2) ;

#ifdef __cplusplus
}
#endif

#endif /* __RTCLIB_H__ */
