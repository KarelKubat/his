/*
 * Central include file for his
 */

#include <getopt.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <sqlite3.h>

/* Type for a parsed command to add. */
typedef struct {
  time_t timestamp;
  char *hash;
  int ac;
  char **av;
} CmdToAdd;

/* Representation of argc/argv, when parsed from a string. */
typedef struct {
  int argc;
  char **argv;
} Args;


/* Type of bindings in a sqlnew() call */
typedef enum {
  INT,
  STR,
} SqlBindType;

/* Context for sqlite3 runs */
typedef struct {
  char const *sql;
  sqlite3_stmt *stmt;
} SqlCtx;

/* Max parsing format that is acceptable and the default */
#define DEFAULT_FORMAT 1
#define MAX_FORMAT     3

/* Data declarations or definitions */
#ifdef DEFINE_DATA
#  define EXTERN
#else
#  define EXTERN extern
#endif
EXTERN int accept_his;         /* Also add cmds that start with 'his' */
EXTERN int count;              /* Number of entries during recent listing */
EXTERN char *db;               /* Database name */
EXTERN sqlite3 *dbconn;        /* Database connection */
EXTERN time_t first_timestamp; /* First timestamps when searching/dumping */
EXTERN int insert_count;       /* #inserts in the db, 0 if no inserts in run */
EXTERN time_t last_timestamp;  /* Last timestamps when searching/dumping */
EXTERN int format;             /* Selected format */
EXTERN int multiargs;          /* --add should expect multi-args */
EXTERN int utc_time;           /* List or import timestamps are UTC */
EXTERN int verbose;            /* Verbosity or not */

/* Functions */
extern void add(int ac, char **av);
extern void db_cleanup(void);
extern void error(char const *fmt, ...);
extern void list_cmds(int ac, char **av);
extern void find(int ac, char **av);
extern char *full_command(int ac, char **av);
extern time_t gm_mktime(struct tm *t);
extern char *hash_string(char const *s);
extern void import_cmds(void);
extern void list_formats(void);
extern int lookup_arg(char const *arg);
extern void msg(char const *fmt, ...);
extern void parse(int ac, char **av, CmdToAdd *cmd);
extern void purge(void);
extern void readme(void);
extern void show_command(int cmd_id, int timestamp);
extern void str2args(char *s, Args *args);
extern time_t str2timestamp(char const *s);
extern void sqlinit(void);
extern int sqlcolint(SqlCtx *ctx, int col);
extern const char *sqlcolstring(SqlCtx *ctx, int col);
extern SqlCtx *sqlnew(char const *sql, int nbindings, ...);
extern int sqlrun(SqlCtx *ctx);
extern void sqlend(SqlCtx *ctx);
extern char *timestamp2str(time_t t);
extern void usage(void);
extern void *xmalloc(size_t sz);
extern void *xrealloc(void *what, size_t sz);
extern char *xsprintf(char const *fmt, ...);
extern char *xstrcat(char *what, char const *extra);
extern char *xstrdup(char const *what);
