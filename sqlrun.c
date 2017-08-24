#include "his.h"

static char **sql_av;     /* Args returned by a select */
static int sql_ac;        /* # of args returned by a select */

static int callback(void *unused, int ac, char **av, char **colnames) {
  int i;
  char *full;

  /* Copy current returns int sql_av and sql_ac */
  if (!ac || !av || !av[0] || !*av[0])
    msg("no results");
  else {
    full = full_command(ac, av);
    msg("results are [%s]", full);
    free(full);

    sql_ac = ac;
    sql_av = (char **)xmalloc((ac + 1) * sizeof(char*));
    for (i = 0; i < ac; i++)
      sql_av[i] = xstrdup(av[i]);
    sql_av[ac] = 0;
  }
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
