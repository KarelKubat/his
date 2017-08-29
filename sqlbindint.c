#include "his.h"

void sqlbindint(sqlite3_stmt *stmt, int val, int pos) {
  if (sqlite3_bind_int(stmt, val, pos + 1) != SQLITE_OK)
    error("failed to bind int [%d] to pos %d: %s",
          val, pos, sqlite3_errmsg(dbconn));
}
