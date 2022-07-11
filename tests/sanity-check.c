/*
  $Header$
  sanity checker.  Looks for the "august tishri" problem
*/

#include "common.h"
#include "greg.h"
#include <stdio.h>
#include <stdlib.h>

char *progname = "sanity-check.c";

/* returns 1 */
int reportIt(long d, const struct hebdate *hebrew) {
  struct tm gregdate = *abs2greg(d);

  printf("%ld : [hm=%d hd=%d hy=%d], [gm=%d gd=%d gy=%d]\n", d, hebrew->mm, hebrew->dd,
         hebrew->yy, gregdate.tm_mon + 1, gregdate.tm_mday, gregdate.tm_year + 1900);
  return 1;
}

int main(int argc, char *argv[]) {
  long interval = 100000;
  long intervalCount = 0;

  long start = 0L;        /* 1 CE */
  long finish = 2595363L; /* ~7060 CE */

  int retVal = 0;
  long d;

  setlinebuf(stdout);

  if (argc > 1) {
    if (3 != argc) {
      fprintf(stderr, "usage: %s [start finish]\n", argv[0]);
      exit(1);
    }
    sscanf(argv[1], "%ld", &start);
    sscanf(argv[2], "%ld", &finish);
  }

  for (d = start; d < finish; d++) {
    if (0 == intervalCount--) {
      printf("   %ld\n", d);
      intervalCount = interval - 1;
    }

    { /* reduce scope for hebrew */
      struct hebdate hebrew = *abs2hebrew(d);
      if (hebrew.dd < 0 || hebrew.mm < 0 || hebrew.yy < 0)
        retVal += reportIt(d, &hebrew);
    }
  }
  printf("found %d failure cases\n", retVal);
  exit(retVal);
}
