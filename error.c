#include "his.h"

void error(char const *fmt, ...) {
  va_list args;

  va_start(args, fmt);
  fprintf(stderr, "his fatal: ");
  vfprintf(stderr, fmt, args);
  fputc('\n', stderr);
  va_end(args);

  /* Rollback the current transaction. Don't do this via sqlrun(), or that
     might trigger error() again. We don't care about a rollback error,
     we can't do anything about it anyway. */
  sqlite3_exec(dbconn, "ROLLBACK TRANSACTION", 0, 0, 0);
  exit(1);
}
