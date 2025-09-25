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


#include "qoraal/qoraal.h"
#include "qoraal/common/rtclib.h"


RTCLIB_TIME_T
rtc_get_time (void)
{
    RTCLIB_TIME_T time = {RTCLOCK_FIRST_TIME} ;
    rtc_localtime (qoraal_current_time(), 0, &time) ;
    return time ;
}

RTCLIB_DATE_T
rtc_get_date (void)
{
    RTCLIB_DATE_T date = {.date = RTCLOCK_FIRST_DATE}  ;
    rtc_localtime (qoraal_current_time(), &date, 0) ;
    return date ;
}

void
rtc_localtime (uint32_t timestamp, RTCLIB_DATE_T* date, RTCLIB_TIME_T* time)
{
    if (date) {
        uint32_t day = timestamp / (24*60*60) + RTCLIB_EPOCH_GDAY;
        date->date = 0 ;
        *date = rtc_get_gdate (day) ;
    }

    if (time) {
        uint32_t seconds = timestamp % (24*60*60) ;
        time->time = 0 ;
        time->hour = seconds / (60*60) ;
        time->minute = (seconds - time->hour*60*60) / 60 ;
        time->second = seconds - time->hour*60*60 - time->minute*60 ;
    }
}

uint32_t
rtc_mktime (RTCLIB_DATE_T date, RTCLIB_TIME_T time)        /* convert date to day number */
{
    uint32_t days = rtc_get_jday (date) ;

    if (days > RTCLIB_EPOCH_GDAY) {
    days -= RTCLIB_EPOCH_GDAY ;
    } else {
        days = 0 ;
    }

    return (days * 24 * 60 * 60 +
                    time.hour * 60 * 60 +
                        time.minute * 60 +
                        time.second) ;
}

/**
 * @brief   Adds the seconds to the current time.
 *
 * @param[in] seconds      seconds to add
 *
 * @api
 */
RTCLIB_TIME_T
rtc_time_add (RTCLIB_TIME_T from, uint32_t seconds)
{
    uint32_t minutes ;
    uint32_t hours ;
    uint32_t days ;

    if (seconds > RTCLIB_SET_ALARM_MAX) {
        DBG_MESSAGE_RTC (DBG_MESSAGE_SEVERITY_ERROR,
                "RTC  : : rtc_time_add %d seconds invalid", seconds);
        //return E_EPARM ;
        seconds = RTCLIB_SET_ALARM_MAX ;
    }

    DBG_MESSAGE_RTC (DBG_MESSAGE_SEVERITY_INFO,
            "RTC   : : rtc_time_add %d seconds to time: %u:%u:%u day: %u", seconds,
            (unsigned int)from.hour, (unsigned int)from.minute,
            (unsigned int)from.second, (unsigned int)from.day);

    minutes = seconds / 60 ;
    hours = minutes / 60 ;
    days = hours / 24 ;

    seconds = seconds % 60 ;
    minutes = minutes % 60 ;
    hours = hours % 24 ;

    from.second += seconds ;
    from.minute += minutes + from.second / 60;
    from.hour += hours + from.minute / 60 ;
    from.day += days + from.hour / 24 ;

    from.second = from.second % 60 ;
    from.minute = from.minute % 60 ;
    from.hour = from.hour % 24 ;

    return from ;
}

/**
 * @brief   Compare two times, is sequental returns positive.
 *          If time2 is before time one returns negative.
 *          If equals return 0.
 *
 * @param[in] time1      start time
 * @param[in] time2      end time
 *
 * @return  seconds since start time
 *
 * @api
 */
int32_t
rtc_time_compare (RTCLIB_TIME_T time1, RTCLIB_TIME_T time2)
{
        if (time2.hour == time1.hour) {
            if (time2.minute == time1.minute) {
                return time2.second - time1.second ;
            } else {
                return (time2.minute - time1.minute) * 60 ;
            }
        } else {
            return (time2.hour - time1.hour) * 60 * 60 ;
        }

}

/**
 * @brief   Calculate the time2 - time1 in seconds.

 *
 * @param[in] time1      start time
 *
 * @return  seconds since time
 *
 * @api
 */
uint32_t
rtc_seconds_diff (RTCLIB_TIME_T time1, RTCLIB_TIME_T time2)
{
    int32_t seconds ;



    seconds = ((time2.hour - time1.hour) * 60 * 60) +
                ((time2.minute - time1.minute) * 60) +
                (time2.second - time1.second) ;

    if (seconds < 0) {

        seconds = ((24 - time1.hour) * 60 * 60) +
                    ((60 - time1.minute) * 60) +
                    (60 - time1.second) ;
        seconds += ((time2.hour) * 60 * 60) +
                    ((time2.minute) * 60) +
                    (time2.second ) ;


    }

    return seconds ;
}

/**
 * @brief   Calculate the time2 - time1 in seconds.

 *
 * @param[in] time1      start time
 *
 * @return  seconds since time
 *
 * @api
 */
int32_t
rtc_days_diff (RTCLIB_DATE_T date1, RTCLIB_DATE_T date2)
{
    if (date1.date == date2.date) {
        return 0 ;
    }

    return rtc_get_jday(date2) - rtc_get_jday(date1) ;
}


/**
 * @brief   Calculate the elapsed seconds since time.

 *
 * @param[in] time1      start time
 *
 * @return  seconds since time
 *
 * @api
 */
#if 1
uint32_t
rtc_seconds_elapsed (RTCLIB_DATE_T date, RTCLIB_TIME_T time)
{
    uint32_t seconds ;
    uint32_t seconds2  ;
    RTCLIB_DATE_T today = rtc_get_date() ;
    RTCLIB_TIME_T now = rtc_get_time () ;

    int cmp = rtc_date_compare(date, today) ;

    if (cmp < 0) {
        return 0 ;
    } else if (cmp == 0) {
        cmp = rtc_time_compare (time, now) ;
        if (cmp <= 0) {
            return 0 ;
        }
    }

    seconds = rtc_mktime (date, time)  ;
    seconds2 = rtc_mktime (today, now)  ;

    if (seconds2 <= seconds) {
        return 0 ;
    }

    return  seconds2 - seconds ;
}
#endif

/**
 * @brief Compares two RTC dates.
 *
 * This function compares two RTC dates and returns the difference between them.
 * The comparison is done based on the year, month, and day of the dates.
 *
 * @param date1 The first RTC date to compare.
 * @param date2 The second RTC date to compare.
 * @return The difference between the two dates. If date2 is greater than date1, a positive value is returned.
 *         If date2 is smaller than date1, a negative value is returned. If the dates are equal, 0 is returned.
 */
int32_t
rtc_date_compare (RTCLIB_DATE_T date1, RTCLIB_DATE_T date2)
{
    if (date2.year == date1.year) {
        if (date2.month == date1.month) {
            return date2.day - date1.day ;
        } else {
            return date2.month - date1.month ;
        }
    } else {
            return date2.year - date1.year ;
    }

}

/**
 * @brief   Calculate the julian day from aRTC date.
 * @description  Julian day is the continuous count of days since the beginning of the Julian Period.
 *                  http://alcor.concordia.ca/~gpkatch
 *               use with hal_rtc_get_gdate to do date arthmetics.

 *
 * @param[in] time1      start time
 *
 * @return  seconds since time
 *
 * @api
 */
int32_t
rtc_get_jday (RTCLIB_DATE_T d)        /* convert date to day number */
{
  int32_t  I,J,K;

  // http://aa.usno.navy.mil/faq/docs/JD_Formula.php

  I = d.year ;
  J = d.month ;
  K = d.day ;

  return K-32075+1461*(I+4800+(J-14)/12)/4+
            367*(J-2-(J-14)/12*12)/12-
            3*((I+4900+(J-14)/12)/100)/4 ;
}

/**
 * @brief   Convert the date back from julian day.
 * @description  The Gregorian calendar is a solar calendar.
 *                  http://alcor.concordia.ca/~gpkatch
 *              use with hal_rtc_get_jday to do date arthmetics.
 *
 * @param[in] time1      start time
 *
 * @return  seconds since time
 *
 * @api
 */
RTCLIB_DATE_T
rtc_get_gdate(uint32_t jday)  /* convert day number to y,m,d format */
{
    RTCLIB_DATE_T pd;
    int32_t  I,J,K,L,N;

    L= jday+68569 ;
    N= 4*L/146097 ;
    L= L-(146097*N+3)/4 ;
    I= 4000*(L+1)/1461001 ;
    L= L-1461*I/4+31 ;
    J= 80*L/2447 ;
    K= L-2447*J/80 ;
    L= J/11 ;
    J= J+2-12*L ;
    I= 100*(N-49)+I+L ;

    pd.year = I ;
    pd.month = J ;
    pd.day = K ;

    return pd;
}

uint32_t
rtc_time (void)        /* convert date to day number */
{
    uint32_t now = qoraal_current_time() ;
    if (now == 0) {
        RTCLIB_DATE_T date = {.date = RTCLOCK_FIRST_DATE}  ;
        RTCLIB_TIME_T time = {.time = RTCLOCK_FIRST_TIME} ;

        return rtc_mktime (date, time) ;

    }
    
    return now ;
}

