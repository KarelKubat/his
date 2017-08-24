#include "his.h"

char *xstrdup(char const *what) {
  char *ret;

  if (!what)
    what = "";

  if (! (ret = strdup(what)) )
    error("out of memory (while allocating %d bytes)", strlen(what));
  return ret;
}
