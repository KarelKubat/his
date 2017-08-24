#include "his.h"

void list_most_recent(void) {
  char **res;
  char *sql, *extra;
  int i;

  sql = xstrdup("SELECT cmd_id, timestamp FROM cmd ORDER BY timestamp");
  if (count) {
      extra = xsprintf(" LIMIT %d", count);
      sql = xstrcat(sql, extra);
      free(extra);
  }
  
  res = sqlrun(sql);
  free(sql);
  if (!res)
    return;

  for (i = 0; res[i]; i += 2)
    show_command(atoi(res[i]), atoi(res[i + 1]));
}
