#include "his.h"

time_t str2timestamp(char const *s) {
  int year, month, day, hour, min, sec;
  struct tm stm;
  time_t ret;

  if (sscanf(s, "%4d-%2d-%2d/%2d:%2d:%2d",
             &year, &month, &day, &hour, &min, &sec) < 6)
    error("timestamp %s doesn't follow required format YYYY-MM-DD/HH:MM:SS",
          s);
  stm.tm_year = year - 1900;
  stm.tm_mon  = month - 1;
  stm.tm_mday = day;
  stm.tm_hour = hour;
  stm.tm_min  = min;
  stm.tm_sec  = sec;

  if (utc_time)
    ret = gm_mktime(&stm);
  else
    ret = mktime(&stm);

  msg("timestring [%s] in %s: timestamp %d",
      s, utc_time? "UTC" : "localtime", ret);
  return ret;

}
