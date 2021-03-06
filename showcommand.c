#include "his.h"

void show_command(int cmd_id, int timestamp) {
  char *line, *cp;
  SqlCtx *ctx;

  line = timestamp2str(timestamp);
  ctx = sqlnew("SELECT   arg from cmd, args, crossref "
               "WHERE    cmd.timestamp = ? AND cmd.cmd_id = ? "
               "AND      cmd.cmd_id = crossref.cmd_id "
               "AND      args.args_id = crossref.args_id "
               "ORDER BY crossref.position",
               2,
               INT, timestamp, INT, cmd_id);
  while ( (sqlrun(ctx) == SQLITE_ROW) ) {
    line = xstrcat(line, " ");
    line = xstrcat(line, sqlcolstring(ctx, 0));
  }
  sqlend(ctx);

  // Newlines are suppressed in the output, else, --import breaks (see
  // importcmds.c).
  while ( (cp = strchr(line, '\n')) )
    *cp = ' ';

  printf("%s\n", line);
  free(line);
}
