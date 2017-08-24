#define DECLARE_DATA

#include "his.h"

typedef enum {
  FIND,       /* Selected when no flag like --add, --export is given */
  ADD,        /* Selected when --add is seen */
  EXPORT,     /* Selected when --export is seen */
  IMPORT,     /* Selected when --import is seen */
  MOSTRECENT, /* Selected when --most-recent is seen */
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
  count = 1;

  /* Supported flags */
  struct option flags[] = {
    { "add",          0, 0, 'a' },
    { "db",           1, 0, 'd' },
    { "count",        1, 0, 'c' },
    { "export",       0, 0, 'e' },
    { "first",        1, 0, 'f' },
    { "format",       1, 0, 'F' },
    { "import",       0, 0, 'i' },
    { "last",         1, 0, 'l' },
    { "list-formats", 0, 0, 'L' },
    { "most-recent",  0, 0, 'r' },
    { "multi-args",   0, 0, 'm' },
    { "verbose",      0, 0, 'v' },
    { "help",         0, 0, 'h' },
    { 0,              0, 0,  0  },
  };
  while ( (opt = getopt_long(argc, argv, "ac:d:ef:F:ilLmrvh?",
                             flags, 0)) > 0 ) {
    switch (opt) {

      case 'a':
        /* Add cmd to history */
        action = ADD;
        break;

      case 'c':
        /* Set count for --most-recent */
        if (!optarg || !*optarg)
          error("missing --count value");
        if ( (count = atoi(optarg)) < 1 )
          error("bad --count value, must be a positive number");
        break;

      case 'd':
        /* sqlite3 db name */
        if (!optarg || !*optarg)
          error("missing --db value");
        free(db);
        db = optarg;
        break;

      case 'e':
        /* Export list to stdout */
        action = EXPORT;
        break;

      case 'f':
        /* First searching/dumping timestamp */
        if (!optarg || !*optarg)
          error("missing --first value");
        first_timestamp = str2timestamp(optarg);
        break;

      case 'F':
        /* Set the format for adding. */
        if (!optarg || !*optarg)
          error("missing --format value");
        if (sscanf(optarg, "%d", &format) < 1)
          error("bad --format value, not a number");
        if (format < 1 || format > MAX_FORMAT)
          error("bad --format value, must be at least 1 and at most %d",
                MAX_FORMAT);
        break;

      case 'i':
        /* Import list from stdin */
        action = IMPORT;
        multiargs = 0;
        break;

      case 'l':
        /* Last searching/dumping timestamp */
        if (!optarg || !*optarg)
          error("missing --last value");
        last_timestamp = str2timestamp(optarg);
        break;

      case 'L':
        /* List formats */
        list_formats();
        break;

      case 'm':
        /* Add should expect multiple args */
        multiargs++;
        break;

      case 'r':
        /* List most recent entry and stop. */
        action = MOSTRECENT;
        break;

      case 'v':
        /* Enable verbose mode */
        verbose++;
        break;

      case 'h':
      case '?':
      default:
        /* Show usage help */
        usage();
        break;
    }
  }

  /* Uninitialized errors */
  if (!db)
    error("failed to determine sqlite3 db path from $HOME or flag --db");
  msg("sqlite3 db is %s", db);
  sqlinit();
  db_cleanup();

  /* Act. */
  switch (action) {
    case FIND:
      if (argc - optind <= 0)
        error("nothing to search for");
      find(argc - optind, argv + optind);
      break;
    case ADD:
      if (argc - optind <= 0)
        error("no command to add");
      add(argc - optind, argv + optind);
      break;
    case EXPORT:
      if (argc - optind != 0)
        error("no arguments expected after --export");
      export_cmds();
      break;
    case IMPORT:
      if (argc - optind != 0)
        error("no arguments expected after --import");\
      format = 2;
      import_cmds();
      break;
    case MOSTRECENT:
      list_most_recent();
      break;
  }

  return 0;
}
