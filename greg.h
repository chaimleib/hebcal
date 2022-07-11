/* Copyright(c) 1994  Danny Sadinoff
  See section COPYING for conditions for redistribution
*/
#ifndef __GREG__
#define __GREG__
#include <time.h>

#define LEAP(x) (!((x) % 4) && (((x) % 100) || !((x) % 400)))
#define DAYS_IN(x) (LEAP((x)) ? 366 : 365)

#define JAN 0
#define FEB 1
#define MAR 2
#define APR 3
#define MAY 4
#define JUN 5
#define JUL 6
#define AUG 7
#define SEP 8
#define OCT 9
#define NOV 10
#define DEC 11

#define SUN 0
#define MON 1
#define TUE 2
#define WED 3
#define THU 4
#define FRI 5
#define SAT 6

extern const char *eMonths[];
extern int MonthLengths[][13];
extern const char *DayNames[];
extern const char *ShortDayNames[];

struct hebdate {
  int mm; /* months: 1-13 */
  int dd; /* day of month 1,30 */
  int yy;
};


int getMonthLength(int year, int month);
int dayOfYear(const struct tm *);
long greg2abs(const struct tm *);
long time2abs(const time_t *);
long tm2abs(const struct tm *);
struct tm *abs2greg(long);
void incDate(struct tm *, long);
int dayOfWeek(const struct tm *);
void setDate(struct tm *);
long day_on_or_before(int, long);
#endif
