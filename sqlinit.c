#include "his.h"
#include "createtablestxt.h"

static void sqlopen() {
  if (sqlite3_open(db, &dbconn))
    error("cannot open db %s: %s", db, sqlite3_errmsg(dbconn));
}

/* If the database doesn't exist yet, create it and the tables in it. */
void sqlinit() {
  struct stat statbuf;
  int db_exists;
  SqlCtx *ctx;

  if (dbconn)
    return;

  db_exists = !stat(db, &statbuf) && statbuf.st_size > 0;
  sqlopen();

  if (db_exists)
    /* Tables have already been set up. */
    msg("db %s already exists (with size %d)", db, statbuf.st_size);
  else {
    /* Create the tables. */
    msg("creating db %s and its tables", db);
    ctx = sqlnew(CREATETABLESTEXT, 0);
    sqlrun(ctx);
    sqlend(ctx);
  }

  /* Start a sqlite3 transaction. An error will roll back, a successful
     finish of main() will commit. */
  ctx = sqlnew("BEGIN TRANSACTION", 0);
  sqlrun(ctx);
  sqlend(ctx);
}
