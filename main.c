#define DECLARE_DATA

#include "his.h"

typedef enum {
  FIND,    /* Selected when no flag like --add, --export is given */
  ADD,     /* Selected when --add is seen */
  EXPORT,  /* Selected when --export is seen */
} Action;

int main(int argc, char **argv) {

  int opt;
  char *home;
  Action action = FIND;

  /* No args at all? */
  if (argc == 1)
    usage();

  /* Initialize globals */
  if ( (home = getenv("HOME")) ) {
    db = xstrdup(home);
    db = xstrcat(db, "/.his.db");
  }
  format = DEFAULT_FORMAT;

  /* Supported flags */
  struct option flags[] = {
    { "add",          0, 0, 'a' },
    { "db",           1, 0, 'd' },
    { "export",       0, 0, 'e' },
    { "first",        1, 0, 'f' },
    { "format",       1, 0, 'F' },
    { "last",         1, 0, 'l' },
    { "list-formats", 0, 0, 'L' },
    { "verbose",      0, 0, 'v' },
    { "help",         0, 0, 'h' },
    { 0,              0, 0,  0  },
  };
  while ( (opt = getopt_long(argc, argv, "ad:ef:F:lLvh?", flags, 0)) > 0 ) {
    switch (opt) {

      /* Add cmd to history */
      case 'a':
        action = ADD;
        break;

      /* sqlite3 db name */
      case 'D':
        if (!optarg || !*optarg)
          error("missing --db value");
        free(db);
        db = optarg;
        break;

      /* Export list */
      case 'e':
        action = EXPORT;
        break;

      /* First searching/dumping timestamp */
      case 'f':
        if (!optarg || !*optarg)
          error("missing --first value");
        first_timestamp = str2timestamp(optarg);
        break;

      /* Set the format for adding. */
      case 'F':
        if (!optarg || !*optarg)
          error("missing --format value");
        if (sscanf(optarg, "%d", &format) < 1)
          error("bad --format value, not a number");
        if (format < 1 || format > MAX_FORMAT)
          error("bad --format value, must be at least 1 and at most %d",
                MAX_FORMAT);
        break;

      /* Last searching/dumping timestamp */
      case 'l':
        if (!optarg || !*optarg)
          error("missing --last value");
        last_timestamp = str2timestamp(optarg);
        break;

      /* List formats */
      case 'L':
        list_formats();
        break;

      /* Enable verbose mode */
      case 'v':
        verbose++;
        break;

      /* Show usage help */
      case 'h':
      case '?':
      default:
        usage();
        break;
    }
  }

  /* Uninitialized errors */
  if (!db)
    error("failed to determine sqlite3 db path from $HOME or flag --db");
  msg("sqlite3 db is %s", db);

  /* Act. */
  switch (action) {
    case FIND:
      if (argc - optind <= 0)
        error("nothing to search for");
      sqlinit();
      find(argc - optind, argv + optind);
      break;
    case ADD:
      if (argc - optind <= 0)
        error("no command to add");
      sqlinit();
      add(argc - optind, argv + optind);
      break;
    case EXPORT:
      if (argc - optind > 0)
        error("no arguments expected after --export");
      sqlinit();
      export();
      break;
  }

  return 0;
}
