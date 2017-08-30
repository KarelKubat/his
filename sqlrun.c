#include "his.h"

int sqlrun(SqlCtx *ctx) {
  int ret;

  ret = sqlite3_step(ctx->stmt);
  if (ret != SQLITE_ROW && ret != SQLITE_DONE)
    error("sql run of [%s] failed with state %d: %s",
          ctx->sql, ret, sqlite3_errmsg(dbconn));
  return ret;
}
