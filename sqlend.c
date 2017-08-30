#include "his.h"

void sqlend(SqlCtx *ctx) {
  // NOTE: We do not sqlite3_finalize(ctx->stmt) because we keep the
  // prepared statement in cache and maybe reuse it (see sqlprepare()
  // in sqlnew.c).
  free(ctx);
}
