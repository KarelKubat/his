#include "his.h"

char *full_command(int ac, char **av) {
  char *ret;
  int i;

  if (ac < 1)
    return 0;
  ret = xstrdup(av[0]);
  for (i = 1; i < ac; i++) {
    ret = xstrcat(ret, " ");
    ret = xstrcat(ret, av[i]);
  }
  return ret;
}
