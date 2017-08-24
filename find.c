#include "his.h"

typedef struct {
  int cmd_id;    /* ID in cmd table */
  int timestamp; /* Timestamp in cmd table */
  int hitcount;  /* Nr. of args this entry matches */
} Result;

void find (int ac, char **av) {
  char *full, *sql, *extra, *stamp;
  sqlite3_stmt *cmd_stmt;
  int i, n, found;
  Result *res = 0;
  int nres = 0;

  full = full_command(ac, av);
  msg("filtering for [%s]", full);
  free(full);

  sql = xstrdup("SELECT cmd.cmd_id, cmd.timestamp "
                "FROM   cmd, crossref, args "
                "WHERE  cmd.cmd_id = crossref.cmd_id "
                "AND    crossref.args_id = args.args_id "
                "AND    args.arg LIKE ?");
  if (first_timestamp) {
    extra = xsprintf(" AND cmd.timestamp >= %d", first_timestamp);
    sql = xstrcat(sql, extra);
    free(extra);
  }
  if (last_timestamp) {
    if (first_timestamp)
      extra = xsprintf(" AND cmd.timestamp <= %d", last_timestamp);
    sql = xstrcat(sql, extra);
    free(extra);
  }
  sqlprepare(sql, &cmd_stmt);

  for (i = 0; i < ac; i++) {
    msg("looking for [%s]", av[i]);
    sqlite3_bind_text(cmd_stmt, 1, av[i], strlen(av[i]), 0);
    while (sqlstep(cmd_stmt) == SQLITE_ROW) {
      found = 0;
      for (n = 0; n < nres; n++)
        if (res[n].cmd_id    == sqlite3_column_int(cmd_stmt, 0) &&
            res[n].timestamp == sqlite3_column_int(cmd_stmt, 1)) {
          res[n].hitcount++;
          found++;
        }
      if (!found) {
        res = (Result *) xrealloc(res, (nres + 1) * sizeof(Result));
        res[nres].cmd_id = sqlite3_column_int(cmd_stmt, 0);
        res[nres].timestamp = sqlite3_column_int(cmd_stmt, 1);
        res[nres].hitcount = 1;
        nres++;
      }
    }
    for (n = 0; n < nres; n++) {
      stamp = gm_timestamp(res[n].timestamp);
      msg("ID %d at %s now has %d hits", res[n].cmd_id, stamp, res[n].hitcount);
      free(stamp);
    }
  }
  sqlite3_finalize(cmd_stmt);

  for (n = 0; n < nres; n++)
    if (res[n].hitcount == ac)
      show_command(res[n].cmd_id, res[n].timestamp);
}
