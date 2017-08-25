#include "his.h"

static char **sql_av;     /* Args returned by a select */
static int sql_ac;        /* # of args returned by a select */

static int callback(void *unused, int ac, char **av, char **colnames) {
  int i;
  char *full;

  /* Copy current returns int sql_av and sql_ac */
  if (!ac || !av || !av[0] || !*av[0]) {
    msg("no results");
    return 0;
  }

  for (i = 0; i < ac; i++) {
    sql_av = (char **)xrealloc(sql_av, (sql_ac + 2) * sizeof(char*));
    sql_av[sql_ac] = xstrdup(av[i]);
    sql_ac++;
    sql_av[sql_ac] = 0;
  }

  full = full_command(sql_ac, sql_av);
  msg("results (so far): [%s]", full);
  free(full);

  return 0;
}

/* Run a SQL statement that is not prepared and should not have any
   bound data. Non-reentrant, not threadsafe, but it doesn't have to. */
char **sqlrun(char const *sql) {
  char *errmsg;
  int i;

  /* Remove previous allocation (if any) */
  if (sql_ac) {
    for (i = 0; i < sql_ac; i++)
      free(sql_av[i]);
    free(sql_av);
    sql_ac = 0;
    sql_av = 0;
  }

  msg("executing [%s]", sql);
  if (sqlite3_exec(dbconn, sql, callback, 0, &errmsg) != SQLITE_OK)
    error(errmsg);

  return sql_av;
}
