#include "his.h"

void show_command(int cmd_id, int timestamp) {
  char *line, *sql, *extra;
  sqlite3_stmt *args_stmt;

  line = gm_timestamp(timestamp);
  sql = xsprintf("SELECT   arg from cmd, args, crossref "
                 "WHERE    cmd.timestamp = %d AND cmd.cmd_id = %d "
                 "AND      cmd.cmd_id = crossref.cmd_id "
                 "AND      args.args_id = crossref.args_id "
                 "ORDER BY crossref.position",
                 timestamp, cmd_id);
  sqlprepare(sql, &args_stmt);
  free(sql);
  while ( (sqlstep(args_stmt) == SQLITE_ROW) ) {
    extra = xsprintf(" %s", sqlite3_column_text(args_stmt, 0));
    line = xstrcat(line, extra);
    free(extra);
  }
  sqlite3_finalize(args_stmt);
  printf("%s\n", line);
  free(line);
}
