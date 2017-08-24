#include "his.h"
#include "createtablestxt.h"

static void sqlopen() {
  if (sqlite3_open(db, &dbconn))
    error("cannot open db %s: %s", db, sqlite3_errmsg(dbconn));
}

/* If the database doesn't exist yet, create it and the tables in it. */
void sqlinit() {
  struct stat statbuf;
  char *errmsg;
  int db_exists;

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
    if (sqlite3_exec(dbconn, CREATETABLESTEXT, 0, 0, &errmsg) != SQLITE_OK)
      error("error while creating tables: %s", errmsg);
  }
}
