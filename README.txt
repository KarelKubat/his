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
    searchable - independent of your terminal or shell session.

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
    shows your 'ls' invocations during the first hour of the year 2017 in
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

    Normally his won't store its own invocations (i.e., anything that starts
    with 'his); it only pollutes the saved list. If you want to, you can use
    flag --accept-his when adding commands to make sure that even such
    invocations end up in the list.

    Adding is 'smart' in the sense that it doesn't duplicate already present
    entries. So you can enter the same command over and over again and `his'
    will only log it as occurring at different timestamps; it won't duplicate
    everything. This even holds true for arguments (parts of a command
    line). E.g., two commands `ls -l /tmp' and 'ls -l /usr' will be stored by
    re-using the parts 'ls' and '-l'. See also DATAMODEL.txt in the
    distribution archive.

  Importing
  ---------

    Exporting and listing can be used together, e.g. to sync lists between
    different machines:
        his -c0 | ssh user@remotesystem his -i
    (For a full export, you want --count=0 or -c0, since the arbitrary default
    is 20.)

    Or, you can ask the 'remote side' what its most recent timestamp is, and
    export only from that point on:
    	TIMESTAMP=$(ssh user@remotesystem his -c1 | awk '{print $1}')
	his -f=${TIMESTAMP?} | ssh user@remotesystem his -i
    Here, flag -c1 means --count=1, which is: list only the one most recent
    command. Flag -f=${TIMESTAMP?} means: list entries on or after $TIMESTAMP.

Making --add work automatically
-------------------------------

  bash
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

  tcsh
  ----

    For tcsh users, edit your ~/.cshrc and put in:

      alias precmd 'his --multi-args --add --format=3 `date +%s` $_'

    tcsh will happily run the precmd alias before displaying the next prompt.
    In this case, this is an invoction to 'his --add' using addition format 3
    (try 'his --list-formats' to see what that is). The information to add is
    the UTC timestamp in seconds and the previous command ($_).
