#include "his.h"

static void run_cleanup(char const *bad_sql_select, char const *delete_sql) {
  char **bad_ids, *bad_id, *sql;

  if (! (bad_ids = sqlrun(bad_sql_select)) )
    return;
  for (bad_id = *bad_ids; bad_id && *bad_id; bad_id++) {
    sql = xsprintf(delete_sql, atoi(bad_id));
    sqlrun(sql);
    free(sql);
  }
}

void db_cleanup() {
  /* Enforce DB consistency. Such errors should of course not occur in the
     first place, but hey. There's crashes and stuff.
     But no bugs. No bugs at all. */

  msg("starting db cleanup");
      
  /* Any cmd_id may only be in the db when it's in both the CMD and in
     the CROSSREF tables. */
  run_cleanup("SELECT cmd_id FROM cmd "
              "WHERE  cmd_id NOT IN (SELECT cmd_id FROM crossref)",
              "DELETE FROM cmd WHERE cmd_id = %d");
  run_cleanup("SELECT cmd_id FROM crossref "
              "WHERE  cmd_id NOT IN (SELECT cmd_id FROM cmd)",
              "DELETE FROM crossref WHERE cmd_id = %d");

  /* Any args_id may only be in the db when it's in both the ARGS and in
     the CROSSREF tables. */
  run_cleanup("SELECT args_id FROM args "
              "WHERE  args_id NOT IN (SELECT args_id FROM crossref)",
              "DELETE FROM args WHERE args_id = %d");
  run_cleanup("SELECT args_id FROM crossref "
              "WHERE  args_id NOT IN (SELECT args_id FROM args)",
              "DELETE FROM crossref WHERE args_id = %d");
}
