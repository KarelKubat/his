#include "his.h"

/* We had an insert. Create or update the count in the housekeeping table. */
static void increment_inserts_count() {
  SqlCtx *readctx, *writectx;

  readctx = sqlnew("SELECT insert_count FROM housekeeping", 0);
  if (sqlrun(readctx) == SQLITE_ROW)
    writectx = sqlnew("UPDATE housekeeping SET insert_count = ?",
                      1,
                      INT, sqlcolint(readctx, 0) + 1);
  else
    writectx = sqlnew("INSERT INTO housekeeping (insert_count) "
                      "VALUES (?)",
                      1,
                      INT, 1);
  sqlrun(writectx);

  sqlend(readctx);
  sqlend(writectx);
}

void add(int ac, char **av) {
  CmdToAdd cmd;
  char *str_stamp, *histignore, *tok;
  int next_cmd_id, i, args_id;
  Args args;
  SqlCtx *readctx, *writectx;
  int found_cmd_id;

  /* In multiargs mode, we want at least some form of a timestamp and
     a command. In singleargs mode, we want exactly one arg. */
  if (!multiargs) {
    /* Split by spaces if --multi-args is given. */
    if (ac != 1)
      error("--add (without --multi-args) needs exactly 1 argument");
    str2args(av[0], &args);
    parse(args.argc, args.argv, &cmd);
  } else {
    /* ac,av must be good */
    if (ac < 2)
      error("--add --multi-args needs at least 2 arguments, some timestamp "
            "and a cmd");
    parse(ac, av, &cmd);
  }

  /* Ignore cmds that start with 'his' unless specified by --accept-his */
  if (!accept_his && !strcmp(cmd.av[0], "his")) {
      msg("command to add starts with 'his', not storing");
      return;
  }

  /* Ignore whatever $HISTIGNORE holds, a-la bash */
  if ( (histignore = getenv("HISTIGNORE")) ) {
    tok = strtok(histignore, ":");
    while (tok) {
      if (!strcmp(cmd.av[0], tok)) {
        msg("command to add starts with '%s', not storing due to $HISTIGNORE",
            tok);
        return;
      }
      tok = strtok(0, ":");
    }
  }

  /* If we already know a db entry with the same timestamp and the
     same hash, then this is a repetition and we don't need to add
     anything.
     If we already know a db entry with the same hash (but a different
     timestamp), then we can just duplicate it in the cmd table and be
     done. */
  readctx = sqlnew("SELECT cmd_id, timestamp "
                   "FROM   cmd "
                   "WHERE  hash = ?",
                   1,
                   STR, cmd.hash);
  found_cmd_id = 0;
  while ( (sqlrun(readctx) == SQLITE_ROW) ) {
    found_cmd_id = sqlcolint(readctx, 0);
    if (sqlcolint(readctx, 1) == cmd.timestamp) {
      str_stamp = timestamp2str(cmd.timestamp);
      msg("entry with hash [%s] was already added at timestamp %s",
          cmd.hash, str_stamp);
      free(str_stamp);
      sqlend(readctx);
      return;
    }
  }
  sqlend(readctx);

  if (found_cmd_id) {
    msg("identical entry occurs at a different stamp, duplicating");
    writectx = sqlnew("INSERT INTO cmd (cmd_id, hash, timestamp) "
                      "VALUES (?, ?, ?)",
                      3,
                      INT, found_cmd_id,
                      STR, cmd.hash,
                      INT, cmd.timestamp);
    sqlrun(writectx);
    sqlend(writectx);
    increment_inserts_count();
    return;
  }

  /* Nope, didn't find it. Will need to add. */
  /* Find the next cmd_id to use. */
  readctx = sqlnew("SELECT MAX(cmd_id) FROM cmd", 0);
  if (sqlrun(readctx) == SQLITE_ROW)
    next_cmd_id = sqlcolint(readctx, 0) + 1;
  else
    next_cmd_id = 1;
  msg("entry will be added at cmd_id=%d", next_cmd_id);
  writectx = sqlnew("INSERT INTO cmd (cmd_id, hash, timestamp) "
                    "VALUES (?, ?, ?)",
                    3,
                    INT, next_cmd_id,
                    STR, cmd.hash,
                    INT, cmd.timestamp);
  sqlrun(writectx);
  sqlend(writectx);

  for (i = 0; i < cmd.ac; i++) {
    args_id = lookup_arg(cmd.av[i]);
    writectx = sqlnew("INSERT INTO crossref (cmd_id, args_id, position) "
                      "VALUES (?, ?, ?)",
                      3,
                      INT, next_cmd_id,
                      INT, args_id,
                      INT, i);
    sqlrun(writectx);
    sqlend(writectx);
  }

  increment_inserts_count();
}
