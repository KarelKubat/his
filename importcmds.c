#include "his.h"

static char *get_stdin_line() {
  char buf[256];
  char *ret = 0, *cp;

  while (1) {
    if (!fgets(buf, sizeof(buf), stdin))
      return ret;
    if (!ret)
      ret = xstrdup(buf);
    else
      ret = xstrcat(ret, buf);
    if ( (cp = strrchr(ret, '\n')) ) {
      *cp = 0;
      return ret;
    }
  }
}

void import_cmds() {
  char *buf;

  while ( (buf = get_stdin_line()) ) {
    int len = strlen(buf);
    if (!len)
      continue;
    if (len < 19)
      error("import line [%s] is not long enough for a timestamp", buf);
    if (len < 21)
      error("import line [%s] is not long enough for a timestamp, space, "
            "and a command", buf);
    if (buf[19] != ' ')
      error("import line [%s] lacks space separator between timestamp "
            "and command");

    add(1, &buf);
    free(buf);
  }
}
