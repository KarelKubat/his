README for clhist

Compiling
---------

    In general it should be enough to type 'make', which creates a local binary
    'his'. Next type 'make install' which puts his in your private binq
    directory (under $HOME). Or use 'BINDIR=/what/ever make install' to install
    to /what/ever.

    The code depends on sqlite3, and you need to have the headers available
    during compilation. On debian-based systems, you may need to 'sudo apt-get
    install libsqlite-dev'. Whichever distribution you use, make sure that
    #include <sqlite3.h> works and that -lsqlite3 finds libsqlite3.a during
    linkage. If you have different paths, then you might need to adapt the
    Makefile.

General Usage
-------------

    his is self-describing, in the sense that if you type 'his' (without
    arguments), you will see what flags and arguments are supported.

  Searching for a command that you ran before
  -------------------------------------------
  
    his is designed to be as easy to use as possible. The general usage is:    
        his ARG1 ARG2 ARG3	
    which will search the command history, and display anything that was typed
    on the commandline earlier having ARG1 and ARG2 and ARG3. SQL-like
    wildcards (% and _) are supported (because the underlying database,
    sqlite3, knows about them). Searching can be limited to a given date/time
    using --first and --last.

  Displaying most recently run commands
  -------------------------------------

    The most recent command can be shown using
        his --most-recent                   # shorthand: his -r
    This also supports a count to show the most N recent commands:
        his --most-recent --count=20        # shorthand: his -rc20

  What adding does
  ----------------
  
    To add commands to the database, his --add .... is used. Normally you want
    this to run automatically, triggered by your shell. How to set that up is
    described below, but you can play around with it from the commandline and
    provided that you supply the right 'parsing format'. This is how his pulls
    out a timestamp and previous commandline from its arguments. The default
    parsing format is the one that bash uses for "history 1". Try    
        his --list-formats
    for an overview.

    Adding is 'smart' in the sense that it doesn't duplicate already present
    entries. So you can add the same command over and over again, without
    polluting the list. Also due to the internal structure, an identical
    command that is added at a different time doesn't duplicate everything,
    only adds that the same thing occurs at different times. See also the
    data model section below.

  Exporting and Importing
  -----------------------

    Exporting the saved list is done using --export. You can limit the list
    using the flags --first and --last. This can be handy when syncing the
    stored commands between different machines:
        his -e | ssh user@remotesystem his -i
    Or, you can ask the 'remote side' what its most recent timestamp is, and
    export only from that point on (see his -r,--most-recent):
    	TIMESTAMP=$(ssh user@remotesystem his -r | awk '{print $1}')
	his -e -f=${TIMESTAMP?} | ssh user@remotesystem his -i
    
Making --add work automatically
-------------------------------

  Bash
  ----
  
    If your shell is bash, edit your ~/.profile (or ~/.bashrc) and put in:
    
    function preprompt() {
       his --add -- "$(history 1)"
    }
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
