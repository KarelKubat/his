#include "his.h"

/* Lookup an arg in table args. If it isn't found yet, insert it.
   Return its ID. */
int lookup_arg(char const *arg) {
  char const select_sql[] = "SELECT args_id FROM args WHERE  arg = '?'";
  char const insert_sql[] = "INSERT INTO args (arg) VALUES (?)";

  sqlite3_stmt *select_stmt, *insert_stmt;

  int args_id;

  sqlprepare(select_sql, &select_stmt);
  sqlite3_bind_text(select_stmt, 1, arg, strlen(arg), 0);
  if ( (sqlstep(select_stmt)) == SQLITE_ROW ) {
    args_id = sqlite3_column_int(select_stmt, 0);
    sqlite3_finalize(select_stmt);
    msg("arg [%s] already found at ID %d", arg, args_id);
    return args_id;
  }

  sqlprepare(insert_sql, &insert_stmt);
  sqlite3_bind_text(insert_stmt, 1, arg, strlen(arg), 0);
  if (sqlstep(insert_stmt) != SQLITE_DONE)
    error("failed to insert [%s] into args: %s", arg, sqlite3_errmsg(dbconn));
  args_id = sqlite3_last_insert_rowid(dbconn);
  sqlite3_finalize(insert_stmt);
  msg("arg [%s] inserted at ID %d", arg, args_id);
  return args_id;
}
