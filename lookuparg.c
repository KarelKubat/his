#include "his.h"

/* Lookup an arg in table args. If it isn't found yet, insert it.
   Return its ID. */
int lookup_arg(char const *arg) {
  SqlCtx *readctx, *writectx;
  int args_id;

  readctx = sqlnew("SELECT args_id FROM args WHERE  arg = ?",
                   1,
                   STR, arg);
  if (sqlrun(readctx) == SQLITE_ROW) {
    args_id = sqlcolint(readctx, 0);
    msg("arg [%s] already found at ID %d", arg, args_id);
    sqlend(readctx);
    return args_id;
  }

  writectx = sqlnew("INSERT INTO args (arg) VALUES (?)",
                    1,
                    STR, arg);
  sqlrun(writectx);
  args_id = sqlite3_last_insert_rowid(dbconn);
  sqlend(writectx);
  msg("arg [%s] inserted at ID %d", arg, args_id);
  return args_id;
}
