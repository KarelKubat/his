#include "his.h"

void purge() {
  sqlite3_stmt
      *select_stmt,
      *args_delete_stmt, *crossref_delete_stmt, *cmd_delete_stmt;
  char *sql, *stamp;
  int cmd_id, cmd_timestamp, args_id, rc;

  /* We need a first and a last timestamp to avoid mistakes. */
  if (!first_timestamp || !last_timestamp)
    error("purge requires --first=FROM and --last=TO");

  /* The procedure is as follows:
     - Select the cmds that fall in the timestamps with the args_ids that
       they point to (via crossref).
     - Select the same args_ids that fall outside of the timestamp range.
     - Subtract.
     - Delete the result.
  */

  args_delete_stmt = sqlprepare("DELETE FROM args "
                                "WHERE args_id = ?");
  crossref_delete_stmt = sqlprepare("DELETE FROM crossref "
                                    "WHERE cmd_id = ? AND args_id = ?");
  cmd_delete_stmt = sqlprepare("DELETE FROM cmd "
                               "WHERE cmd_id = ? AND timestamp = ?");

  sql = xsprintf("SELECT  cmd.cmd_id, cmd.timestamp, args.args_id "
                 "FROM    cmd, crossref, args "
                 "WHERE   cmd.cmd_id=crossref.cmd_id "
                 "  AND   args.args_id=crossref.args_id "
                 "  AND   cmd.timestamp >= %d AND cmd.timestamp <= %d "
                 "  AND   args.args_id NOT IN "
                 "(SELECT args.args_id "
                 " FROM   cmd, crossref, args "
                 " WHERE  cmd.cmd_id=crossref.cmd_id "
                 " AND    args.args_id=crossref.args_id "
                 " AND    (cmd.timestamp < %d OR cmd.timestamp > %d))",
                 first_timestamp, last_timestamp,
                 first_timestamp, last_timestamp);
  select_stmt = sqlprepare(sql);
  free(sql);
  while (sqlstep(select_stmt) == SQLITE_ROW) {
    cmd_id        = sqlite3_column_int(select_stmt, 0);
    cmd_timestamp = sqlite3_column_int(select_stmt, 1);
    args_id       = sqlite3_column_int(select_stmt, 2);
    stamp         = gm_timestamp(cmd_timestamp);

    msg("purge candidate: cmd_id=%d, args_id=%d, timestamp=%s",
        cmd_id, args_id, stamp);
    free(stamp);

    /* This args_id occurs only within the timestamp */
    sqlbindint(args_delete_stmt, args_id, 0);
    if (sqlstep(args_delete_stmt) != SQLITE_DONE)
      error("failed to delete from args table where args_id=%d: %s",
            args_id, sqlite3_errmsg(dbconn));

    /* cmd entry can go. */
    sqlbindint(cmd_delete_stmt, cmd_id, 0);
    sqlbindint(cmd_delete_stmt, cmd_timestamp, 1);
    if (sqlstep(cmd_delete_stmt) != SQLITE_DONE)
      error("failed to delete from cmd where cmd_id=%d and timestamp=%d: %s",
            cmd_id, cmd_timestamp, sqlite3_errmsg(dbconn));

    /* crossref is cleaned up last as it has foreign keys */
    sqlbindint(crossref_delete_stmt, cmd_id, 0);
    sqlbindint(crossref_delete_stmt, args_id, 1);
    if (sqlstep(crossref_delete_stmt) != SQLITE_DONE)
      error("failed to delete from crossref where cmd_id=%d "
            "and args_id=%d: %s",
            cmd_id, args_id, sqlite3_errmsg(dbconn));
  }

  sqlite3_finalize(select_stmt);
  sqlite3_finalize(args_delete_stmt);
  sqlite3_finalize(crossref_delete_stmt);

  /* Whatever is left over should just be duplicates of other cmd entries
     occurring in the given timeframe. We can delete those and ignore
     constraint errors. */
  sql = xsprintf("SELECT cmd_id, timestamp "
                 "FROM   cmd "
                 "WHERE timestamp >= %d AND timestamp <= %d",
                 first_timestamp, last_timestamp);
  select_stmt = sqlprepare(sql);
  free(sql);

  while (sqlstep(select_stmt) == SQLITE_ROW) {
    cmd_id        = sqlite3_column_int(select_stmt, 0);
    cmd_timestamp = sqlite3_column_int(select_stmt, 1);

    sqlbindint(cmd_delete_stmt, cmd_id, 0);
    sqlbindint(cmd_delete_stmt, cmd_timestamp, 1);

    if ( (rc = sqlstep(cmd_delete_stmt)) != SQLITE_DONE &&
         rc != SQLITE_CONSTRAINT)
      error("failed to delete from cmd where cmd_id=%d and timestamp=%d: %s",
            cmd_id, cmd_timestamp, sqlite3_errmsg(dbconn));
  }

  sqlite3_finalize(cmd_delete_stmt);
}
