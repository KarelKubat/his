#include "his.h"

int sqlstep(sqlite3_stmt *stmt) {
  int ret;

  ret = sqlite3_step(stmt);
  if (ret != SQLITE_ROW && ret != SQLITE_DONE)
    error("sql execution failed with state %d: %s",
          ret, sqlite3_errmsg(dbconn));
  return ret;
}
