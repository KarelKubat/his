#include "his.h"

char *xsprintf(char const *fmt, ...) {
  char *buf = 0, *ret;
  size_t sz;
  va_list args;

  va_start(args, fmt);
  for (sz = 512; sz < 51200; sz += 512) {
    free(buf);
    buf = (char*)xmalloc(sz);
    if (vsnprintf(buf, sz, fmt, args) < (int)sz) {
      ret = xstrdup(buf);
      free(buf);
      return ret;
    }
  }
  error("string expansion overrun, stopped at %d bytes", sz);
  return 0;
}
