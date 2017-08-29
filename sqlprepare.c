#include "his.h"

sqlite3_stmt *sqlprepare(char const *sql) {
  sqlite3_stmt *ret;

  msg("preparing [%s]", sql);
  if ( (sqlite3_prepare(dbconn, sql, strlen(sql), &ret, 0) != SQLITE_OK) )
    error("failed to prepare [%s]: %s", sql, sqlite3_errmsg(dbconn));
  return ret;
}
