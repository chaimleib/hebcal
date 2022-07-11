/*
   Hebcal - A Jewish Calendar Generator
   Copyright (C) 1994-2004  Danny Sadinoff
   Portions Copyright (c) 2002 Michael J. Radwin. All Rights Reserved.

   https://github.com/hebcal/hebcal

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2
   of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

   Danny Sadinoff can be reached at
   danny@sadinoff.com
 */

#include "greg.h"
#include "danlib.h"
#include "myerror.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

/* greg.c gregorian calendar module for hebrew calendar program
   By Danny Sadinoff
   (C) 1992

 */

const char *eMonths[] = {"UNUSED",  "January",  "February", "March",  "April",
                         "May",     "June",     "July",     "August", "September",
                         "October", "November", "December"};

int MonthLengths[][13] = {{0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
                          {0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}};

int getMonthLength(int year, int month) {
  if (month < 0 || month > 13) {
    return 0;
  }
  return MonthLengths[LEAP(year)][month];
}

const char *ShortDayNames[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};

/*
 *Return the day number within the year of the date DATE.
 *For example, dayOfYear({1,1,1987}) returns the value 1
 *while dayOfYear({12,31,1980}) returns 366.
 */

int dayOfYear(const struct tm *date) {
  int dOY = date->tm_mday + 31 * date->tm_mon;
  if (date->tm_mon > FEB) {
    dOY -= (4 * date->tm_mon + 27) / 10;
    if (LEAP(date->tm_year))
      dOY++;
  }
  return dOY;
}

/*
 * Given a Gregorian date, returns the "absolute date", which is the number of
 * days elapsed between the Gregorian date 12/31/1 BC and DATE.
 * The Gregorian date Sunday, December 31, 1 BC is imaginary.
 */
long int greg2abs(const struct tm *date) {
  const long y = (long)date->tm_year + 1900L;
  return ((long)dayOfYear(date) /* days this year */
          + 365L * (y - 1)      /* + days in prior years */
          + ((y - 1) / 4        /* + Julian Leap years */
             - (y - 1) / 100    /* - century years */
             + (y - 1) / 400)); /* + Gregorian leap years */
}

/*
 * See the footnote on page 384 of ``Calendrical Calculations, Part II:
 * Three Historical Calendars'' by E. M. Reingold,  N. Dershowitz, and S. M.
 * Clamen, Software--Practice and Experience, Volume 23, Number 4
 * (April, 1993), pages 383-404 for an explanation.
 */
struct tm *abs2greg(long theDate) {
  static struct tm ret;
  int day, year, month, mlen;
  long int d0, n400, d1, n100, d2, n4, d3, n1;

  d0 = theDate - 1L;
  n400 = d0 / 146097L;
  d1 = d0 % 146097L;
  n100 = d1 / 36524L;
  d2 = d1 % 36524L;
  n4 = d2 / 1461L;
  d3 = d2 % 1461L;
  n1 = d3 / 365L;

  day = (int)((d3 % 365L) + 1L);
  year = (int)(400L * n400 + 100L * n100 + 4L * n4 + n1);

  if (4L == n100 || 4L == n1) {
    ret.tm_mon = 11;
    ret.tm_mday = 31;
    ret.tm_year = year - 1900;
    return &ret;
  } else {
    year++;
    month = 1;
    while ((mlen = MonthLengths[LEAP(year)][month]) < day) {
      day -= mlen;
      month++;
    }
    ret.tm_year = year - 1900;
    ret.tm_mon = month - 1;
    ret.tm_mday = day;
    return &ret;
  }
}

void incDate(struct tm *dt, long n) /* increments dt by n days */
{
  *dt = *abs2greg(greg2abs(dt) + n);
}

int dayOfWeek(const struct tm *date) /* sunday = 0 */
{
  return (int)(greg2abs(date) % 7L);
}

void setDate(struct tm *date) {
  time_t secs = time(NULL);
  date = localtime(&secs);
}

/** Returns the absolute date of the DAYNAME on or before absolute DATE.
 * DAYNAME=0 means Sunday, DAYNAME=1 means Monday, and so on.


 * Note: Applying this function to d+6 gives us the DAYNAME on or after an
 * absolute day d.  Similarly, applying it to d+3 gives the DAYNAME nearest to
 * absolute date d, applying it to d-1 gives the DAYNAME previous to absolute
 * date d, and applying it to d+7 gives the DAYNAME following absolute date d.

**/
long day_on_or_before(int day_of_week, long date) {
  return date - ((date - (long)day_of_week) % 7L);
}

/** (defun calendar-nth-named-day (n dayname month year &optional day)
   "The date of Nth DAYNAME in MONTH, YEAR before/after optional DAY.
   A DAYNAME of 0 means Sunday, 1 means Monday, and so on.  If N<0,
   return the Nth DAYNAME before MONTH DAY, YEAR (inclusive).
   If N>0, return the Nth DAYNAME after MONTH DAY, YEAR (inclusive).

   If DAY is omitted, it defaults to 1 if N>0, and MONTH's last day otherwise.
   (calendar-gregorian-from-absolute
   (if (> n 0)
   (+ (* 7 (1- n))
   (calendar-dayname-on-or-before
   dayname
   (+ 6 (calendar-absolute-from-gregorian
   (list month (or day 1) year)))))
   (+ (* 7 (1+ n))
   (calendar-dayname-on-or-before
   dayname
   (calendar-absolute-from-gregorian
   (list month
   (or day (calendar-last-day-of-month month year))
   year)))))))
 */
