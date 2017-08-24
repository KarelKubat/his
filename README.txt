README for clhist

Compiling
    In general it should be enough to type 'make', which creates a local
    binary 'his'.

    The code depends on sqlite3, and you need to have the headers
    available during compilation. On debian-based systems, you may
    need to 'sudo apt-get install libsqlite-dev'. Whichever
    distribution you use, make sure that #include <sqlite3.h> works
    and that -lsqlite3 finds libsqlite3.a during linkage. If you have
    different paths, then you might need to adapt the Makefile.
    
    