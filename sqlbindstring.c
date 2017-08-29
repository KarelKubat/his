#include "his.h"

void sqlbindstring(sqlite3_stmt *stmt, char const *str, int pos) {
  if (sqlite3_bind_text(stmt, pos + 1, str, strlen(str), 0) != SQLITE_OK)
    error("failed to bind string [%s] to pos %d: %s",
          str, pos, sqlite3_errmsg(dbconn));
}
