#include "his.h"

void add(int ac, char **av) {
  CmdToAdd cmd;
  char **sqlret;
  char *sql, *str_stamp;
  time_t found_timestamp;
  int next_cmd_id, i, args_id;
  Args args;

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
  
  /* If we already know a db entry with the same timestamp and the
     same hash, then this is a repetition and we don't need to add
     anything.
     If we already know a db entry with the same hash (but a different
     timestamp), then we can just duplicate it in the cmd table and be
     done. */
  sql = xsprintf("SELECT cmd_id, timestamp "
                 "FROM   cmd "
                 "WHERE  hash = %d", cmd.hash);
  sqlret = sqlrun(sql);
  free(sql);
  if (sqlret) {
    found_timestamp = atoi(sqlret[1]);
    if (found_timestamp == cmd.timestamp) {
      msg("entry was already added");
      return;
    }
    str_stamp = gm_timestamp(found_timestamp);
    msg("identical entry occurs at %s, duplicating for new timestamp",
        str_stamp);
    free(str_stamp);
    sql = xsprintf("INSERT INTO cmd (cmd_id, hash, timestamp) "
                   "VALUES (%d, %d, %d)",
                   atoi(sqlret[0]), cmd.hash, cmd.timestamp);
    sqlrun(sql);
    free(sql);
    return;
  }

  /* Nope, didn't find it. Will need to add. */
  /* Find the next cmd_id to use. */
  sql = (char*)"SELECT MAX(cmd_id) FROM cmd";
  if (! (sqlret = sqlrun(sql)) )
    next_cmd_id = 1;
  else {
    next_cmd_id = atoi(sqlret[0]) + 1;
  }
  msg("entry will be added at cmd_id=%d", next_cmd_id);
  sql = xsprintf("INSERT INTO cmd (cmd_id, hash, timestamp) "
                 "VALUES (%d, %d, %d)", next_cmd_id, cmd.hash, cmd.timestamp);
  sqlrun(sql);
  free(sql);

  for (i = 0; i < cmd.ac; i++) {
    args_id = lookup_arg(cmd.av[i]);
    sql = xsprintf("INSERT INTO crossref (cmd_id, args_id, position) "
                   "VALUES (%d, %d, %d)", next_cmd_id, args_id, i);
    sqlrun(sql);
    free(sql);
  }
}
