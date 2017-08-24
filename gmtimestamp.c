#include "his.h"

char *gm_timestamp(time_t t) {
  struct tm *tmptr;

  tmptr = gmtime(&t);
  return xsprintf("%4.4d-%2.2d-%2.2d/%2.2d:%2.2d:%2.2d",
                  tmptr->tm_year + 1900, tmptr->tm_mon + 1, tmptr->tm_mday,
                  tmptr->tm_hour, tmptr->tm_min, tmptr->tm_sec);
}
