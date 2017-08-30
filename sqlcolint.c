#include "his.h"

int sqlcolint(SqlCtx *ctx, int col) {
  return sqlite3_column_int(ctx->stmt, col);
}
