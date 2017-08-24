#include "his.h"

void sqlprepare(char const *sql, sqlite3_stmt **stmt) {
  msg("preparing [%s]", sql);

  if ( (sqlite3_prepare(dbconn, sql, strlen(sql), stmt, 0) != SQLITE_OK) )
    error("failed to prepare [%s]: %s", sql, sqlite3_errmsg(dbconn));
}
