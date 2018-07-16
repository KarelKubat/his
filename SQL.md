# SQL Usage

The `his` package contains useful wrappers for sqlite3. You might want to
repurpose it for your own programs, or you might want to know how it works
incase you want to extend `his`. Here's how it works.

## General Flow

After initialization, run `sqlnew()` to obtain a context that is re-used for
later execution. This internally creates a prepared statement. The parameters
are:

*  A string holding the SQL statement, having `?` placeholders for
   bind variables

*  The number of bindings that follow

*  The bindings itself, where each binding is a 2-parameter setting: either
   `INT,<int-value>` or `STR,<string-value>`.

Example:

```c
SqlCtx *ctx = sqlnew("SELECT someint, somestring FROM mytable "
                     "WHERE fullname = ? AND unix_timestamp <= ?",

                     2,                   // 2 bindings to follow
                     STR, "John Doe",     // 1st binding: string
                     INT, time());        // 2nd binding: int
```


Once a context is obtained, use `sqlrun()` to execute the statement. In the case
of a `SELECT` statement, `SQLITE_ROW` indicates that there are more rows to
fetch. In that case, two functions can be called to fetch data from a `SELECT`:

*  `sqlcolint(ctx, N)`: returns the Nth value as an int. Use N==0 for the
   first column, N==1 for the second one, etc.

*  `sqlcolstring(ctx, N)` returns the Nth value as a string.

When `sqlrun()` returns `SQLITE_DONE` then this indicates that there are no more
rows. This is also the only return value for `INSERT` or `UPDATE` statements.

Other sqlite3 return values are caught internally and an `error()` is issued.
Example:

```c
while (sqlrun(ctx) == SQLITE_ROW) {
    int  someint     = sqlcolint(ctx, 0);
    char *somestring = sqlcolstring(ctx, 1);
    ...
}
```

 Once done, call

 ```c
sqlend(ctx);
```

to release the resources.

Here is a second example with an `INSERT` statement. The return value of
`sqlrun()` doesn't need to be checked: this either succeeds, or it
errors-out.

```c
ctx = sqlnew("INSERT INTO mytable "
                 "(someint, somestring, fullname, unix_timestamp) "
             "VALUES (?, ?, ?, ?)",
             4,                       // 4 bindings to follow
             INT, 12,                 // 1: an int
             STR, "hello world",      // 2: a string
             STR, "Jane Doe",         // 3: a string
             INT, time());            // 4: an int
sqlrun(ctx);
sqlend(ctx);
```

## Prepared Statements Are Reused

Don't be afraid to run `sqlnew()` for the same SQL string over and over again,
so it can be bound to different variables. Internally, the prepared statement is
cached and reused. So `sqlnew()` will only prepare the statement when the sql is
seen for the first time; otherwise, only bindings are performed.

## What Is Missing

This approach only implements what `his` needs. There is room for
improvement. In particular:

*  The database connection `dbconn` is expected as a global variable.

*  All functions call `error()` when something goes wrong. This exits the
   program.

*   There is only support for binding or fetching ints or strings, no
    doubles and the like.

*  `BEGIN TRANSACTION` vs. `COMMIT` or `ROLLBACK` is hardwired on a
   per-execution basis. This means in detail: When the program starts,
   `sqlinit()` is called to set up the database connection and to `BEGIN
   TRANSACTION`.  When the program exits successfully (see `main.c`), a `COMMIT`
   is done.  In all cases where an error is seen (see error.c) a `ROLLBACK
   TRANSACTION` is done.

*  Binding of variables could be moved to `sqlrun()` instead of `sqlnew()`. This
   wouldn't speed up anything, but might be more intuitive.
