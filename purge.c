#include "his.h"

void purge() {
  sqlite3_stmt *args_stmt, *crossref_stmt, *cmd_stmt;
  int within_stamps, stamp;

  error("--purge is not yet implemented");

  /* We need a first and a last timestamp to avoid mistakes. */
  if (!first_timestamp || !last_timestamp)
    error("purge requires --first=FROM and --last=TO");

  /* Work backwards. For each ARG, fetch all CROSSREFS and CMDS.
     If the CMD entry is in the timeframe, the ARG, the CROSSREF and the CMD
     may go. */
  sqlprepare("SELECT args_id FROM args", &args_stmt);
  while (sqlstep(args_stmt) == SQLITE_ROW) {
    sqlprepare("SELECT cmd_id FROM crossref WHERE args_id = ?", &crossref_stmt);
    sqlite3_bind_int(crossref_stmt, 1, sqlite3_column_int(args_stmt, 0));
    while (sqlstep(crossref_stmt) == SQLITE_ROW) {
      sqlprepare("SELECT timestamp FROM cmd WHERE cmd_id = ?", &cmd_stmt);
      sqlite3_bind_int(cmd_stmt, 1, sqlite3_column_int(crossref_stmt, 0));
      within_stamps = 1;
      while (sqlstep(cmd_stmt) == SQLITE_ROW) {
        stamp = sqlite3_column_int(cmd_stmt, 0);
        if (stamp < first_timestamp || stamp > last_timestamp) {
          within_stamps = 0;
          break;
        }
      }
      sqlite3_finalize(cmd_stmt);
    }
  }
}
<
