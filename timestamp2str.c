#include "his.h"

static char *tm_to_string(struct tm *tmptr) {
  return xsprintf("%4.4d-%2.2d-%2.2d/%2.2d:%2.2d:%2.2d",
                  tmptr->tm_year + 1900, tmptr->tm_mon + 1, tmptr->tm_mday,
                  tmptr->tm_hour, tmptr->tm_min, tmptr->tm_sec);
}


char *timestamp2str(time_t t) {
  if (utc_time)
    return tm_to_string(gmtime(&t));
  return tm_to_string(localtime(&t));
}
