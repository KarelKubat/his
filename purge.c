#include "his.h"

void purge() {
  SqlCtx *readctx, *xrefctx, *cmdctx, *argsctx;
  char *stamp;
  int cmd_id, cmd_timestamp, args_id;

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

  readctx = sqlnew("SELECT  cmd.cmd_id, cmd.timestamp, args.args_id "
                   "FROM    cmd, crossref, args "
                   "WHERE   cmd.cmd_id=crossref.cmd_id "
                   "  AND   args.args_id=crossref.args_id "
                   "  AND   cmd.timestamp >= ? AND cmd.timestamp <= ? "
                   "  AND   args.args_id NOT IN "
                   "(SELECT args.args_id "
                   " FROM   cmd, crossref, args "
                   " WHERE  cmd.cmd_id=crossref.cmd_id "
                   " AND    args.args_id=crossref.args_id "
                   " AND    (cmd.timestamp < ? OR cmd.timestamp > ?))",
                   4,
                   INT, first_timestamp, INT, last_timestamp,
                   INT, first_timestamp, INT, last_timestamp);
  while (sqlrun(readctx) == SQLITE_ROW) {
    cmd_id        = sqlcolint(readctx, 0);
    cmd_timestamp = sqlcolint(readctx, 1);
    args_id       = sqlcolint(readctx, 2);
    stamp         = timestamp2str(cmd_timestamp);

    msg("purge candidate: cmd_id=%d, args_id=%d, timestamp=%s",
        cmd_id, args_id, stamp);
    free(stamp);

    /* This args_id occurs only within the timestamp */
    argsctx = sqlnew("DELETE FROM args WHERE args_id = ?",
                     1,
                     INT, args_id);
    sqlrun(argsctx);
    sqlend(argsctx);

    /* cmd entry can go. */
    cmdctx = sqlnew("DELETE FROM cmd "
                    "WHERE cmd_id = ? AND timestamp = ?",
                    2,
                    INT, cmd_id, INT,
                    cmd_timestamp);
    sqlrun(cmdctx);
    sqlend(cmdctx);

    /* crossref is cleaned up last as it has foreign keys */
    xrefctx = sqlnew("DELETE FROM crossref "
                     "WHERE cmd_id = ? AND args_id = ?",
                     2,
                     INT, cmd_id,
                     INT, args_id);
    sqlrun(xrefctx);
    sqlend(xrefctx);
  }

  /* Whatever is left over should just be duplicates of other cmd entries
     occurring in the given timeframe. We can delete those and ignore
     constraint errors. */
  cmdctx = sqlnew("DELETE FROM cmd "
                  "WHERE  timestamp >= ? AND timestamp <=? "
                  "AND    cmd_id NOT IN (SELECT cmd_id FROM crossref)",
                 first_timestamp, last_timestamp);
  sqlrun(cmdctx);
  sqlend(cmdctx);
}
