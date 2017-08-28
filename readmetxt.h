/* Auto-generated from README.txt. Do not edit. */
/* Instead adapt README.txt and run 'make readmetxt.h' */

#define READMETEXT \
    "What it\'s about\n"\
    "---------------\n"\
    "\n"\
    "    Have you ever wanted to keep your shell commands stored for your\n"\
    "    convenience indefinitely, so you can search for anything you typed, even\n"\
    "    years ago? As a sysadmin you might have solved some problem a year ago\n"\
    "    using some obscure seldom-used tool and now you\'re wondering what that\n"\
    "    invocation was. Or do you want to be able to sync your shell history across\n"\
    "    machines, so that previously entered commands on one system can be seen in\n"\
    "    the history list on another system?\n"\
    "\n"\
    "    With his you can. It stores each command that you enter in a database,\n"\
    "    right as the next prompt appears. The history list is immediately\n"\
    "    searchable.\n"\
    "\n"\
    "Compiling\n"\
    "---------\n"\
    "\n"\
    "    In general it should be enough to type \'make\', which creates a local binary\n"\
    "    \'his\'. Next type \'make install\' which puts his in your private binq\n"\
    "    directory (under $HOME). Or use \'BINDIR=/what/ever make install\' to install\n"\
    "    to /what/ever.\n"\
    "\n"\
    "    The code depends on sqlite3, and you need to have the headers and the\n"\
    "    development lib available during compilation and linking. On debian-based\n"\
    "    systems, you may need to \'sudo apt-get install libsqlite-dev\'. Whichever\n"\
    "    distribution you use, make sure that #include <sqlite3.h> works and that\n"\
    "    -lsqlite3 finds libsqlite3.a. If you have different paths, then you might\n"\
    "    need to adapt the Makefile.\n"\
    "\n"\
    "General Usage\n"\
    "-------------\n"\
    "\n"\
    "    his is self-describing, in the sense that if you type either of\n"\
    "        his --help\n"\
    "	his -h\n"\
    "	his -?\n"\
    "    you will see what flags and arguments are supported.\n"\
    "\n"\
    "  Searching for a command that you ran before\n"\
    "  -------------------------------------------\n"\
    "\n"\
    "    Typing just\n"\
    "        his\n"\
    "    will display the most recent commands, by default limited to the last 20.\n"\
    "    This is similar to the bash builtin \'history\'. You can limit the\n"\
    "    selection by providing a timeframe (--first and --last) and/or a count\n"\
    "    (--count).\n"\
    "\n"\
    "    To find specific commands, use the form\n"\
    "        his ARG1 ARG2 ARG3\n"\
    "    which will search the command history, and display anything that was typed\n"\
    "    on the commandline earlier having ARG1 and ARG2 and ARG3. SQL-like\n"\
    "    wildcards (% and _) are supported (because the underlying database,\n"\
    "    sqlite3, knows about them). Searching can be limited to a given date/time\n"\
    "    using --first and --last. If you want to see only the last three\n"\
    "    occurrences, use --count=3 (shorthand -c3). Example:\n"\
    "        his --first=2017-01-01/00:00:00 --last=2017-01-01/01:00:00 ls\n"\
    "    shows your \'ls\' invocations during the first hour of the year 2017 in\n"\
    "    the GMT timezone.\n"\
    "\n"\
    "  What adding does\n"\
    "  ----------------\n"\
    "\n"\
    "    To add commands to the database, his --add .... is used. Normally you want\n"\
    "    this to run automatically, triggered by your shell. How to set that up is\n"\
    "    described below, but you can play around with it from the commandline and\n"\
    "    provided that you supply the right \'parsing format\'. This is how his pulls\n"\
    "    out a timestamp and previous commandline from its arguments. The default\n"\
    "    parsing format is the one that bash uses when you\n"\
    "        export HISTTIMEFORMAT=\"[%Y-%m-%d %H:%M:%S %z] \"\n"\
    "    You can see that history format by running the Bash builtin command\n"\
    "    \'history\'. Try for an overview of formats.\n"\
    "\n"\
    "    Normally his won\'t store its own invocations (i.e., anything that starts\n"\
    "    with \'his); it only pollutes the saved list. If you want to, you can use\n"\
    "    flag --accept-his when adding commands to make sure that even such\n"\
    "    invocations end up in the list.\n"\
    "\n"\
    "    Adding is \'smart\' in the sense that it doesn\'t duplicate already present\n"\
    "    entries. So you can enter the same command over and over again and `his\'\n"\
    "    will only log it as occurring at different timestamps; it won\'t duplicate\n"\
    "    everything. This even holds true for arguments (parts of a command\n"\
    "    line). E.g., two commands `ls -l /tmp\' and \'ls -l /usr\' will be stored by\n"\
    "    re-using the parts \'ls\' and \'-l\'. See also the data model section below.\n"\
    "\n"\
    "  Importing\n"\
    "  ---------\n"\
    "\n"\
    "    Exporting and listing can be used together, e.g. to sync lists between\n"\
    "    different machines:\n"\
    "        his -c0 | ssh user@remotesystem his -i\n"\
    "    (For a full export, you want --count=0 or -c0, since the arbitrary default\n"\
    "    is 20.)\n"\
    "\n"\
    "    Or, you can ask the \'remote side\' what its most recent timestamp is, and\n"\
    "    export only from that point on:\n"\
    "    	TIMESTAMP=$(ssh user@remotesystem his -c1 | awk \'{print $1}\')\n"\
    "	his -f=${TIMESTAMP?} | ssh user@remotesystem his -i\n"\
    "    Here, flag -c1 means --count=1, which is: list only the one most recent\n"\
    "    command. Flag -f=${TIMESTAMP?} means: list entries on or after $TIMESTAMP.\n"\
    "\n"\
    "Making --add work automatically\n"\
    "-------------------------------\n"\
    "\n"\
    "  bash\n"\
    "  ----\n"\
    "\n"\
    "    If your shell is bash, edit your ~/.profile (or ~/.bashrc) and put in:\n"\
    "\n"\
    "    function preprompt() {\n"\
    "       his --add -- \"$(history 1)\"\n"\
    "    }\n"\
    "    export HISTTIMEFORMAT=\"[%Y-%m-%d %H:%M:%S %z] \"\n"\
    "    PROMPT_COMMAND=preprompt\n"\
    "\n"\
    "    What this does, is instruct bash that there is a $PREPROMPT_COMMAND to run,\n"\
    "    which is in this case a bash function preprompt. That function runs his\n"\
    "    --add. The lone flag -- makes sure that his doesn\'t consider the remainder\n"\
    "    as flags; ie. whatever is in \"$(history 1)\". The statement \"$(history 1)\"\n"\
    "    will ask bash to evaluate this as the last command.\n"\
    "\n"\
    "    You can of course put more stuff in the preprompt function; anything\n"\
    "    that bash should run for you before displaying the next prompt.\n"\
    "\n"\
    "    Et voila, your commands are saved into a sqlite database called\n"\
    "    $HOME/.his.db.\n"\
    "\n"\
    "  tcsh\n"\
    "  ----\n"\
    "\n"\
    "    For tcsh users, edit your ~/.cshrc and put in:\n"\
    "\n"\
    "      alias precmd \'his --multi-args --add --format=3 `date +%s` $_\'\n"\
    "\n"\
    "    tcsh will happily run the precmd alias before displaying the next prompt.\n"\
    "    In this case, this is an invoction to \'his --add\' using addition format 3\n"\
    "    (try \'his --list-formats\' to see what that is). The information to add is\n"\
    "    the UTC timestamp in seconds and the previous command ($_).\n"\
    "\n"\
    "Data model and storage\n"\
    "----------------------\n"\
    "\n"\
    "    his tries to be smart about storing invocations, their arguments, and their\n"\
    "    timestamps. There are three tables. You can inspect everything if you have\n"\
    "    the CLI \'sqlite3\' and when you run \'sqlite3 ~/.his.db\' (assuming that you\n"\
    "    are not overruling the default by having your own -d MYDB flag).\n"\
    "\n"\
    "    If you want to see what tables are created and exactly how, see file\n"\
    "    createtables.txt in the source archive.\n"\
    "\n"\
    "    Here\'s a functional description of the tables:\n"\
    "\n"\
    "    1. Table CMD: What command was entered when?\n"\
    "       - Column CMD_ID: an integer that is used to index table CROSSREF. It is\n"\
    "         not unique; it may occur multiple times when the same command occurs\n"\
    "         under different timestamps.\n"\
    "       - Column TIMESTAMP: The timestamp at which this command occurred. If an\n"\
    "         identical command is entered at a new timestamp (e.g. say you run \"ls\n"\
    "	 -l\" at different times), then the same CMD_ID is used, but with a\n"\
    "	 different TIMESTAMP.\n"\
    "       - Column HASH: A hash computed over the full entered commmand, used to\n"\
    "         identify previously stored commands so that de-duplication can be\n"\
    "         applied.\n"\
    "\n"\
    "    2. Table ARGS: What args do we know?\n"\
    "       - Column ARGS_ID: the auto-incremented ID of the row.\n"\
    "       - Column ARG: one argument. E.g. a command \'ls -ltr /tmp\' will cause 3\n"\
    "         entries in ARGS: \'ls\', \'-ltr\', and \'/tmp\'.\n"\
    "\n"\
    "    3. Table CROSSREF: binding the two together\n"\
    "       This table links CMD entries to ARGS entries to make the picture\n"\
    "       complete.\n"\
    "       - Column CMD_ID:   references an entry in CMD, e.g. answering \'when was\n"\
    "       	 this entered\'.\n"\
    "       - Column ARGS_ID:  references an argument in the stored command.\n"\
    "       - Column POSITION: says which ARG_ID comes first, which comes second,\n"\
    "         etc.\n"\
    "       The whole idea of CROSSREF is to deduplicate ARGS. E.g., consider the\n"\
    "       three commands:\n"\
    "         ls -l /tmp\n"\
    "	 ls -ltr\n"\
    "	 ls /tmp\n"\
    "       have only four distinct strings: \"ls\", \"-l\", \"/tmp\", and \"-ltr\". The\n"\
    "       tables and their contents will in this case have 3 CMD rows, but only 4\n"\
    "       ARGS rows. The crosstable CROSSREF defines which ARG-strings are bound\n"\
    "       to which CMD and in which order.\n"\
    "\n"
