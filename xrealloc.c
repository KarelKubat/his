#include "his.h"

void *xrealloc(void *what, size_t sz) {
  if (!what)
    return xmalloc(sz);
  if (! (what = realloc(what, sz)) )
    error("out of memory (while reallocating to %d bytes)", sz);
  return what;
}
