# Data model and storage

`his` tries to be smart about storing invocations, their arguments, and their
timestamps. There are three tables. You can inspect everything if you have the
CLI `sqlite3` and when you run `sqlite3 ~/.his.db` (assuming that you are not
overruling the default by having your own `-d MYDB` flag).

If you want to see what tables are created and exactly how, see file
`createtables.txt` in the source archive.

Here's a functional description of the tables:

1. Table `cmd`: What command was entered when?

   *  Column `cmd_id`: an integer that is used to index table CROSSREF. It is
      not unique; it may occur multiple times when the same command occurs under
      different timestamps.

   *  Column `timestamp`: The timestamp at which this command occurred. If an
      identical command is entered at a new timestamp (e.g. say you run "ls -l"
      at different times), then the same `cmd_id` is used, but with a different
      `timestamp`.

   *  Column `hash`: A hash computed over the full entered commmand, used to
      identify previously stored commands so that de-duplication can be applied.

1. Table `args`: What args do we know?

   *  Column `args_id`: the auto-incremented ID of the row.

   *  Column `arg`: one argument. E.g. a command "ls -ltr /tmp" will cause 3
      entries in `args`: `ls`, `-ltr`, and `/tmp`.

1. Table `crossref`: binding the two together. This table links `cmd` entries to
   `args` entries to make the picture complete.

   *  Column `cmd_id`: references an entry in `cmd`, e.g. answering 'when was
   	  this entered'.
   *  Column `args_id`:  references an argument in the stored command.
   *  Column `position`: says which `args_id` comes first, which comes second,
      etc.

   The whole idea of `crossref` is to deduplicate ARGS. E.g., consider the
   three commands:

   ```shell
   ls -l /tmp
   ls -ltr
   ls /tmp
   ```

   These have only four distinct strings: "ls", "-l", "/tmp", and "-ltr". The
   tables and their contents will in this case have 3 `cmd` rows, but only 4
   `args` rows. The crosstable `crossref` defines which `arg`-strings are bound
   to which `cmd` and in which order.
