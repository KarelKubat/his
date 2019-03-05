/* DECLARE_DATA makes his.h define, not just declare variables that
   are marked EXTERN */
#define DEFINE_DATA

#include "his.h"

typedef enum {
  LIST,       /* Selected when no flag like --add, --export is given */
  ADD,        /* Selected when --add is seen */
  IMPORT,     /* Selected when --import is seen */
  PURGE,      /* Selected when --purge is seen */
} Action;

int main(int argc, char **argv) {

  int opt;
  char *home;
  Action action = LIST;

  /* Initialize globals */
  if ( (home = getenv("HOME")) ) {
    db = xstrdup(home);
    db = xstrcat(db, "/.his.db");
  }
  format = DEFAULT_FORMAT;
  /* A negative count means: not initialized. It is overruled in
     list_cmds() unless it's set by a flag. */
  count = -1;

  /* Supported flags */
  struct option flags[] = {
    { "accept-his",          0, 0, 'A' },
    { "add",                 0, 0, 'a' },
    { "db",                  1, 0, 'd' },
    { "count",               1, 0, 'c' },
    { "first",               1, 0, 'f' },
    { "format",              1, 0, 'F' },
    { "import",              0, 0, 'i' },
    { "last",                1, 0, 'l' },
    { "list-formats",        0, 0, 'L' },
    { "multi-args",          0, 0, 'm' },
    { "purge",               0, 0, 'p' },
    { "report-missing-args", 0, 0, 'R' },
    { "utc",                 0, 0, 'u' },
    { "verbose",             0, 0, 'v' },
    { "help",                0, 0, 'h' },
    { 0,                     0, 0,  0  },
  };
  while ( (opt = getopt_long(argc, argv, "aAc:d:f:F:il:Lmpvuh?",
                             flags, 0)) > 0 ) {
    switch (opt) {

      case 'a':
        /* Add cmd to history */
        action = ADD;
        break;

      case 'A':
        /* Store cmds that start with 'his' */
        accept_his++;
        break;

      case 'c':
        /* Set count for --most-recent */
        if (!optarg || !*optarg)
          error("missing --count value");
        if ( (count = atoi(optarg)) < 0 )
          error("bad --count value, must be a non-negative number");
        break;

      case 'd':
        /* sqlite3 db name */
        if (!optarg || !*optarg)
          error("missing --db value");
        free(db);
        db = optarg;
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

      case 'p':
        /* Purge stuff */
        action = PURGE;
        break;

      case 'R':
	/* Report missing args ON */
	report_missing_args++;
	break;

      case 'u':
        /* Import or export timestamps are UTC */
        utc_time++;
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

  /* Act. */
  switch (action) {
    case LIST:
      list_cmds(argc - optind, argv + optind);
      break;
    case ADD:
      if (argc - optind <= 0)
        error("no command to add");
      add(argc - optind, argv + optind);
      break;
      break;
    case IMPORT:
      if (argc - optind != 0)
        error("no arguments expected after --import");\
      format = 2;
      import_cmds();
      break;
    case PURGE:
      purge();
      break;
  }

  /* Run a consistency checker if we have hit <magicnumber> of inserts
     since the last checker run. */
  db_cleanup();

  /* A transaction was started in sqlinit(). We got here because we didn't
     error() out so it's safe to commit. We are not running this through
     sqlrun() because that might error out. If this COMMIT doesn't work then
     we can't do much about it anyway. */
  sqlite3_exec(dbconn, "COMMIT TRANSACTION", 0, 0, 0);

  return 0;
}
