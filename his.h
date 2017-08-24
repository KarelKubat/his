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
  int hash;
  int ac;
  char **av;
} CmdToAdd;

/* Max parsing format that is acceptable and the default */
#define DEFAULT_FORMAT 1
#define MAX_FORMAT     3

/* Data declarations or definitions */
#ifdef DECLARE_DATA
#  define EXTERN
#else
#  define EXTERN extern
#endif
EXTERN char *db;               /* Database name */
EXTERN time_t first_timestamp; /* First timestamps when searching/dumping */
EXTERN time_t last_timestamp;  /* Last timestamps when searching/dumping */
EXTERN int format;             /* Selected format */
EXTERN sqlite3 *dbconn;        /* Database connection */
EXTERN int verbose;            /* Verbosity or not */

/* Functions */
extern void add(int ac, char **av);
extern void error(char const *fmt, ...);
extern void export(void);
extern void find(int ac, char **av);
extern char *full_command(int ac, char **av);
extern time_t gm_mktime(struct tm *t);
extern char *gm_timestamp(time_t t);
extern void list_formats(void);
extern int lookup_arg(char const *arg);
extern void msg(char const *fmt, ...);
extern void parse(int ac, char **av, CmdToAdd *cmd);
extern void show_command(int cmd_id, int timestamp);
extern time_t str2timestamp(char const *s);
extern void sqlinit(void);
extern void sqlprepare(char const *sql, sqlite3_stmt **stmt);
extern char **sqlrun(char const *sql);
extern int sqlstep(sqlite3_stmt *stmt);
extern void usage(void);
extern void *xmalloc(size_t sz);
extern void *xrealloc(void *what, size_t sz);
extern char *xsprintf(char const *fmt, ...);
extern char *xstrcat(char *what, char const *extra);
extern char *xstrdup(char const *what);
