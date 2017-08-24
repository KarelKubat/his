#include "his.h"
#include "usagetxt.h"

void usage() {
  fprintf(stderr, USAGETEXT, VER, AUTHOR, YEARS, URL);
  exit(1);
}
