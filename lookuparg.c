#include "his.h"

/* Lookup an arg in table args. If it isn't found yet, insert it.
   Return its ID. */
int lookup_arg(char const *arg) {
  char const select_sql[] = "SELECT args_id FROM args WHERE  arg = ?";
  char const insert_sql[] = "INSERT INTO args (arg) VALUES (?)";

  sqlite3_stmt *select_stmt, *insert_stmt;

  int args_id;

  select_stmt = sqlprepare(select_sql);
  sqlbindstring(select_stmt, arg, 0);
  if ( (sqlstep(select_stmt)) == SQLITE_ROW ) {
    args_id = sqlite3_column_int(select_stmt, 0);
    sqlite3_finalize(select_stmt);
    msg("arg [%s] already found at ID %d", arg, args_id);
    return args_id;
  }
  sqlite3_finalize(insert_stmt);

  insert_stmt = sqlprepare(insert_sql);
  sqlbindstring(insert_stmt, arg, 0);
  if (sqlstep(insert_stmt) != SQLITE_DONE)
    error("failed to insert [%s] into args: %s", arg, sqlite3_errmsg(dbconn));
  args_id = sqlite3_last_insert_rowid(dbconn);
  sqlite3_finalize(insert_stmt);
  msg("arg [%s] inserted at ID %d", arg, args_id);
  return args_id;
}
