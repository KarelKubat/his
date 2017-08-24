/* #include "his.h"

typedef struct {
  int cmd_id;
  int timestamp;
} Cmd;

void list_most_recent(void) {
  char **res;
  char *sql;
  int i;
  Cmd *cmd = 0;
  int ncmd = 0;

  sql = xsprintf("SELECT cmd_id, timestamp "
                 "FROM cmd ORDER BY timestamp DESC "
                 "LIMIT %d", count);
  res = sqlrun(sql);
  free(sql);
  if (!res)
    return;

  for (i = 0; res[i]; i += 2) {
    if (!ncmd)
      cmd = xmalloc(Cmd);
    else
      cmd = xrealloc(Cmd, (ncmd + 1) * sizeof(Cmd));

    show_command(atoi(res[i]), atoi(res[i + 1]));
}

*/
#include "his.h"

void list_most_recent(void) {
  char **res;
  char *sql;
  int i;

  sql = xsprintf("SELECT cmd_id, timestamp "
                 "FROM cmd ORDER BY timestamp DESC "
                 "LIMIT %d", count);
  res = sqlrun(sql);
  free(sql);
  if (!res)
    return;

  for (i = 0; res[i]; i += 2)
    show_command(atoi(res[i]), atoi(res[i + 1]));
}
