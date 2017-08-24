#include "his.h"

char *xstrcat(char *what, char const *extra) {
  what = xrealloc(what, strlen(what) + strlen(extra) + 1);
  strcat(what, extra);
  return what;
}
