#include "his.h"

void db_cleanup() {
  SqlCtx *ctx;
  /* Enforce DB consistency. Such errors should of course not occur in the
     first place, but hey. There's crashes and stuff.
     But no bugs. No bugs at all. Never bugs.

     We only do this if the count of inserts in the db has reached the magic
     number 1962. Then we reset the counter. Why 1962? Because it's magic.
  */

  if (insert_count < 1962)
    return;

  msg("starting db cleanup");

  /* Any cmd_id may only be in the db when it's in both the CMD and in
     the CROSSREF tables. */
  ctx = sqlnew("DELETE FROM cmd "
               "WHERE  cmd_id NOT IN (SELECT cmd_id FROM crossref)", 0);
  sqlrun(ctx);
  sqlend(ctx);

  /* Any args_id may only be in the db when it's in both the ARGS and in
     the CROSSREF tables. */
  ctx = sqlnew("DELETE FROM args "
               "WHERE  args_id NOT IN (SELECT args_id FROM args)", 0);
  sqlrun(ctx);
  sqlend(ctx);

  /* Reset the insert count. */
  ctx = sqlnew("UPDATE housekeeping SET insert_count = 0", 0);
  sqlrun(ctx);
  sqlend(ctx);
}
