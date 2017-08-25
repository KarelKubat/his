README for clhist

What it's about
---------------

    Have you ever wanted to keep your shell commands stored for your
    convenience indefinitely, so you can search for anything you typed, even
    years ago? As a sysadmin you might have solved some problem a year ago
    using some obscure seldom-used tool and now you're wondering what that
    invocation was. Or do you want to be able to sync your shell history across
    machines, so that previously entered commands on one system can be seen in
    the history list on another system?

    With his you can. It stores each command that you enter in a database,
    right as the next prompt appears. The history list is immediately
    searchable.

Compiling
---------

    In general it should be enough to type 'make', which creates a local binary
    'his'. Next type 'make install' which puts his in your private binq
    directory (under $HOME). Or use 'BINDIR=/what/ever make install' to install
    to /what/ever.

    The code depends on sqlite3, and you need to have the headers and the
    development lib available during compilation and linking. On debian-based
    systems, you may need to 'sudo apt-get install libsqlite-dev'. Whichever
    distribution you use, make sure that #include <sqlite3.h> works and that
    -lsqlite3 finds libsqlite3.a. If you have different paths, then you might
    need to adapt the Makefile.

General Usage
-------------

    his is self-describing, in the sense that if you type either of
        his --help
	his -h
	his -?
    you will see what flags and arguments are supported.

  Searching for a command that you ran before
  -------------------------------------------

    Typing just
        his
    will display the most recent commands, by default limited to the last 20.
    This is similar to the bash builtin 'history'. You can limit the
    selection by providing a timeframe (--first and --last) and/or a count
    (--count).

    To find specific commands, use the form
        his ARG1 ARG2 ARG3
    which will search the command history, and display anything that was typed
    on the commandline earlier having ARG1 and ARG2 and ARG3. SQL-like
    wildcards (% and _) are supported (because the underlying database,
    sqlite3, knows about them). Searching can be limited to a given date/time
    using --first and --last. If you want to see only the last three
    occurrences, use --count=3 (shorthand -c3). Example:
        his --first=2017-01-01/00:00:00 --last=2017-01-01/01:00:00 ls
    shows your 'ls' invocations during the first our of the year 2017 in
    the GMT timezone.
    
  What adding does
  ----------------

    To add commands to the database, his --add .... is used. Normally you want
    this to run automatically, triggered by your shell. How to set that up is
    described below, but you can play around with it from the commandline and
    provided that you supply the right 'parsing format'. This is how his pulls
    out a timestamp and previous commandline from its arguments. The default
    parsing format is the one that bash uses when you
        export HISTTIMEFORMAT="[%Y-%m-%d %H:%M:%S %z] "
    You can see that history format by running the Bash builtin command
    'history'. Try for an overview of formats.

    Normally his won't store its own invocations; it only pollutes the saved
    list. If you want to, you can use flag --accept-his when adding commands to
    make sure that even such invocations end up in the list.

    Adding is 'smart' in the sense that it doesn't duplicate already present
    entries. So you can add the same command over and over again, without
    polluting the list. Also due to the internal structure, an identical
    command that is added at a different time doesn't duplicate everything,
    only adds that the same thing occurs agin but at a different timestamp. See
    also the data model section below.

  Importing
  ---------

    Exporting and listing can be used together, e.g. when syncing stored
    commands between different machines:
        his -c0 | ssh user@remotesystem his -i
    (Remember that for a full export, you want --count=0 or -c0, since the
    arbitrary default is 20.)

    Or, you can ask the 'remote side' what its most recent timestamp is, and
    export only from that point on:
    	TIMESTAMP=$(ssh user@remotesystem his -c1 | awk '{print $1}')
	his -f=${TIMESTAMP?} | ssh user@remotesystem his -i
    Here, flag -c1 means --count=1, which is: list only the one most recent
    command. Flag -f=${TIMESTAMP?} means: list entries on or after $TIMESTAMP.

Making --add work automatically
-------------------------------

  Bash
  ----

    If your shell is bash, edit your ~/.profile (or ~/.bashrc) and put in:

    function preprompt() {
       his --add -- "$(history 1)"
    }
    export HISTTIMEFORMAT="[%Y-%m-%d %H:%M:%S %z] "
    PROMPT_COMMAND=preprompt

    What this does, is instruct bash that there is a $PREPROMPT_COMMAND to run,
    which is in this case a bash function preprompt. That function runs his
    --add. The lone flag -- makes sure that his doesn't consider the remainder
    as flags; ie. whatever is in "$(history 1)". The statement "$(history 1)"
    will ask bash to evaluate this as the last command.

    You can of course put more stuff in the preprompt function; anything
    that bash should run for you before displaying the next prompt.

    Et voila, your commands are saved into a sqlite database called
    $HOME/.his.db.

Data model and storage
----------------------

    his tries to be smart about storing invocations, their arguments, and their
    timestamps. There are three tables. You can inspect everything if you have
    the CLI 'sqlite3' and when you run 'sqlite3 ~/.his.db' (assuming that you
    are not overruling the default by having your own -d MYDB flag).

    If you want to see what tables are created and exactly how, see file
    createtables.txt in the source archive.

    Here's a functional description of the tables:

    1. Table CMD: What command was entered when?
       - Column CMD_ID: an integer that is used to index table CROSSREF. It is
         not unique; it may occur multiple times when the same command occurs
         under different timestamps.
       - Column TIMESTAMP: The timestamp at which this command occurred. If an
         identical command is entered at a new timestamp (e.g. say you run "ls
	 -l" at different times), then the same CMD_ID is used, but with a
	 different TIMESTAMP.
       - Column HASH: A hash computed over the full entered commmand, used to
         identify previously stored commands so that de-duplication can be
         applied.

    2. Table ARGS: What args do we know?
       - Column ARGS_ID: the auto-incremented ID of the row.
       - Column ARG: one argument. E.g. a command 'ls -ltr /tmp' will cause 3
         entries in ARGS: 'ls', '-ltr', and '/tmp'.

    3. Table CROSSREF: binding the two together
       This table links CMD entries to ARGS entries to make the picture
       complete.
       - Column CMD_ID:   references an entry in CMD, e.g. answering 'when was
       	 this entered'.
       - Column ARGS_ID:  references an argument in the stored command.
       - Column POSITION: says which ARG_ID comes first, which comes second,
         etc.
       The whole idea of CROSSREF is to deduplicate ARGS. E.g., consider the
       three commands:
         ls -l /tmp
	 ls -ltr
	 ls /tmp
       have only four distinct strings: "ls", "-l", "/tmp", and "-ltr". The
       tables and their contents will in this case have 3 CMD rows, but only 4
       ARGS rows. The crosstable CROSSREF defines which ARG-strings are bound
       to any CMD and in which order.
