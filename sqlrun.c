#include "his.h"

int sqlrun(SqlCtx *ctx) {
  int ret, i;

  // Retry database locked conditions.
  for (i = 0; i < 5; i++) {
    ret = sqlite3_step(ctx->stmt);
    if (ret != SQLITE_BUSY)
      break;
    sleep(1);
  }
  if (ret != SQLITE_ROW && ret != SQLITE_DONE)
    error("sql run of [%s] failed with state %d: %s",
          ctx->sql, ret, sqlite3_errmsg(dbconn));
  return ret;
}
