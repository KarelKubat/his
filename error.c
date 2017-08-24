#include "his.h"

void error(char const *fmt, ...) {
  va_list args;

  va_start(args, fmt);
  fprintf(stderr, "his fatal: ");
  vfprintf(stderr, fmt, args);
  fputc('\n', stderr);
  va_end(args);

  exit(1);
}
