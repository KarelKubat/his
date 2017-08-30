#include "his.h"

// Cache of prepared statements.
typedef struct {
  char const *sql;
  sqlite3_stmt *stmt;
} PreparedStmt;

static PreparedStmt *preparedstmt;
int npreparedstmt;

static sqlite3_stmt *sqlprepare(char const *sql) {
  int i;
  sqlite3_stmt *stmt;

  for (i = 0; i < npreparedstmt; i++)
    if (!strcmp(sql, preparedstmt[i].sql))
      return preparedstmt[i].stmt;

  msg("preparing [%s]", sql);
  if ( (sqlite3_prepare(dbconn, sql, strlen(sql), &stmt, 0) != SQLITE_OK) )
    error("failed to prepare [%s]: %s", sql, sqlite3_errmsg(dbconn));
  preparedstmt =
      (PreparedStmt *)xrealloc(preparedstmt,
                               (npreparedstmt + 1) * sizeof(PreparedStmt));
  preparedstmt[npreparedstmt].sql = xstrdup(sql);
  preparedstmt[npreparedstmt].stmt = stmt;
  return stmt;
}

static void sqlbindint(sqlite3_stmt *stmt, int val, int pos,
                       char const *sql) {
  msg("binding int [%d] to pos %d in [%s]", val, pos, sql);
  // Why is sqlite3_bind_...() one-based instead of being sanely zero-based?
  if (sqlite3_bind_int(stmt, pos + 1, val) != SQLITE_OK)
    error("failed to bind int [%d] to pos %d in [%s]: %s",
          val, pos, sql, sqlite3_errmsg(dbconn));
}

static void sqlbindstring(sqlite3_stmt *stmt, char const *str, int pos,
                          char const *sql) {
  msg("binding string [%s] to pos %d in [%s]", str, pos, sql);
  if (sqlite3_bind_text(stmt, pos + 1, str, strlen(str), 0) != SQLITE_OK)
    error("failed to bind string [%s] to pos %d in [%s]: %s",
          str, pos, sql, sqlite3_errmsg(dbconn));
}

SqlCtx *sqlnew(char const *sql, int nbindings, ...) {
  va_list args;
  SqlCtx *ctx;
  int i;
  SqlBindType bindtype;

  va_start(args, nbindings);
  ctx = (SqlCtx *)xmalloc(sizeof(SqlCtx));
  ctx->sql = sql;
  ctx->stmt = sqlprepare(ctx->sql);

  for (i = 0; i < nbindings; i++) {
    bindtype = va_arg(args, int);
    switch (bindtype) {
      case INT:
        sqlbindint(ctx->stmt, va_arg(args, int), i, sql);
        break;
      case STR:
        sqlbindstring(ctx->stmt, va_arg(args, char*), i, sql);
        break;
    }
  }

  va_end(args);
  return ctx;
}
