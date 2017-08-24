#include "his.h"

void export_cmds() {
  char *sql, *extra;
  sqlite3_stmt *cmd_stmt;

  sql = xstrdup("SELECT cmd_id, timestamp FROM cmd");

  /* Add --first if given */
  if (first_timestamp) {
    extra = xsprintf(" WHERE timestamp >= %d", first_timestamp);
    sql = xstrcat(sql, extra);
    free(extra);
  }

  /* Add --last if given */
  if (last_timestamp) {
    if (first_timestamp)
      sql = xstrcat(sql, " AND ");
    else
      sql = xstrcat(sql, " WHERE ");
    extra = xsprintf(" timestamp <= %d", last_timestamp);
    sql = xstrcat(sql, extra);
    free(extra);
  }
  
  sql = xstrcat(sql, " ORDER BY timestamp");

  /* Add countlimit if given */
  if (count) {
      extra = xsprintf(" LIMIT %d", count);
      sql = xstrcat(sql, extra);
      free(extra);
  }
  
  sqlprepare(sql, &cmd_stmt);
  free(sql);

  while ( (sqlstep(cmd_stmt) == SQLITE_ROW) )
    show_command(sqlite3_column_int(cmd_stmt, 0),
                 sqlite3_column_int(cmd_stmt, 1));
  sqlite3_finalize(cmd_stmt);
}
