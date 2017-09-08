#include "his.h"

typedef struct {
  int cmd_id;    /* ID in cmd table */
  int timestamp; /* Timestamp in cmd table */
  int hitcount;  /* Nr. of args this entry matches */
} Result;

/* The invocation was without timestamps, without a count, without args, so a
   full dump.  We can just order by timestamp, without constraints, and show
   all. */
static void list_all() {
  SqlCtx *ctx;

  ctx = sqlnew("SELECT cmd_id, timestamp FROM cmd ORDER BY TIMESTAMP",
               0);
  while ( (sqlrun(ctx) == SQLITE_ROW) )
    show_command(sqlcolint(ctx, 0), sqlcolint(ctx, 1));
  sqlend(ctx);
}

/* The invocation was without args to find, but with a count or with
   timestamps. We must order by the timestamp in reverse to get the most recent
   matching entries, and then flip it for display. */
static void list_by_timestamps() {
  SqlCtx *ctx;
  char *sql, *extra;
  Result *res = 0;
  int nres = 0, i;

  sql = xstrdup("SELECT cmd_id, timestamp FROM cmd");

  /* Add --first if given */
  if (first_timestamp) {
    extra = xsprintf(" WHERE timestamp >= %d", first_timestamp);
    sql = xstrcat(sql, extra);
    free(extra);
  }

  /* Add --last if given */
  if (last_timestamp) {
    if (first_timestamp)
      sql = xstrcat(sql, " AND ");
    else
      sql = xstrcat(sql, " WHERE ");
    extra = xsprintf(" timestamp <= %d", last_timestamp);
    sql = xstrcat(sql, extra);
    free(extra);
  }

  /* We need to fetch the last ones and then display them in reverse order. */
  sql = xstrcat(sql, " ORDER BY timestamp DESC");

  /* Add countlimit if given */
  if (count) {
      extra = xsprintf(" LIMIT %d", count);
      sql = xstrcat(sql, extra);
      free(extra);
  }

  ctx = sqlnew(sql, 0);
  free(sql);

  while ( (sqlrun(ctx) == SQLITE_ROW) ) {
    if (!nres)
      res = (Result *) xmalloc(sizeof(Result));
    else
      res = (Result *) xrealloc(res, (nres + 1) * sizeof(Result));
    res[nres].cmd_id    = sqlcolint(ctx, 0);
    res[nres].timestamp = sqlcolint(ctx, 1);
    nres++;
  }
  sqlend(ctx);

  for (i = nres - 1; i >= 0; i--)
    show_command(res[i].cmd_id, res[i].timestamp);
  free(res);
}

/* qsort callback */
static int compar(const void *a, const void *b) {
  Result *ra = (Result *)a;
  Result *rb = (Result *)b;
  return ra->timestamp - rb->timestamp;
}

/* The invocation was with args to find. */
static void list_with_finding(int ac, char **av) {
  char *full, *sql, *extra, *stamp;
  SqlCtx *ctx;
  int i, n, found;
  Result *res = 0;
  int nres = 0, nshown = 0;

  full = full_command(ac, av);
  msg("filtering for [%s]", full);
  free(full);

  sql = xstrdup("SELECT cmd.cmd_id, cmd.timestamp "
                "FROM   cmd, crossref, args "
                "WHERE  cmd.cmd_id = crossref.cmd_id "
                "AND    crossref.args_id = args.args_id "
                "AND    args.arg LIKE ?");
  if (first_timestamp) {
    extra = xsprintf(" AND cmd.timestamp >= %d", first_timestamp);
    sql = xstrcat(sql, extra);
    free(extra);
  }
  if (last_timestamp) {
    if (first_timestamp)
      extra = xsprintf(" AND cmd.timestamp <= %d", last_timestamp);
    sql = xstrcat(sql, extra);
    free(extra);
  }
  sql = xstrcat(sql, " ORDER BY cmd.timestamp");

  for (i = 0; i < ac; i++) {
    msg("looking for [%s]", av[i]);
    ctx = sqlnew(sql, 1,
                 STR, av[i]);
    while (sqlrun(ctx) == SQLITE_ROW) {
      found = 0;
      for (n = 0; n < nres; n++)
        if (res[n].cmd_id    == sqlcolint(ctx, 0) &&
            res[n].timestamp == sqlcolint(ctx, 1)) {
          res[n].hitcount++;
          found++;
        }
      if (!found) {
        res = (Result *) xrealloc(res, (nres + 1) * sizeof(Result));
        res[nres].cmd_id    = sqlcolint(ctx, 0);
        res[nres].timestamp = sqlcolint(ctx, 1);
        res[nres].hitcount = 1;
        nres++;
      }
    }
    for (n = 0; n < nres; n++) {
      stamp = timestamp2str(res[n].timestamp);
      msg("ID %d at %s now has %d hits", res[n].cmd_id, stamp, res[n].hitcount);
      free(stamp);
    }
  }
  sqlend(ctx);

  /* Sort the results by timestamp. */
  qsort(res, nres, sizeof(Result), compar);

  /* Remove entries that don't hit all required args, ie. entries
     where hitcount < ac
  */
  n = 0;
  while (n < nres)
    if (res[n].hitcount < ac) {
      memmove(res + n, res + n + 1, (nres - n - 1) * sizeof(Result));
      nres--;
    } else {
      n++;
    }

  /* Display starts at 0 if:
     - There is no count specified
     - There is a count specified, but we have less hits (nres)
     Else, display starts at nres-count
  */
  if (!count || nres - count <= 0)
    n = 0;
  else
    n = nres - count;
  for (; n < nres; n++)
    if (res[n].hitcount == ac)
      show_command(res[n].cmd_id, res[n].timestamp);
  free(res);
}

void list_cmds(int ac, char **av) {
  /* If the count is uninitialized (-1) then apply the default 20. */
  if (count < 0)
    count = 20;

  if (ac)
    list_with_finding(ac, av);
  else {
    if (first_timestamp || last_timestamp || count)
      list_by_timestamps();
    else
      list_all();
  }
}
