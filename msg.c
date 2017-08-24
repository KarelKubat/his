#include "his.h"

void msg(char const *fmt, ...) {
  va_list args;

  if (!verbose)
    return;
  va_start(args, fmt);
  printf("his debug: ");
  vprintf(fmt, args);
  putc('\n', stdout);
  va_end(args);
}
