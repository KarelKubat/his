#include "his.h"

void str2args(char *s, Args *args) {
  char *tok;

  args->argc = 0;

  tok = strtok(s, " ");
  while (tok) {
    if (*tok != ' ') {
      if (!args->argc)
        args->argv = (char **) xmalloc(sizeof(char*));
      else
        args->argv = (char **) xrealloc(args->argv,
                                        (args->argc + 1) * sizeof(char *));
      args->argv[args->argc] = xstrdup(tok);
      args->argc++;
    }
    tok = strtok(0, " ");
  }
}
