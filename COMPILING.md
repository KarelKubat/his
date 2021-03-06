# Compiling his

In general it should be enough to type

```shell
make his
```

which creates a local binary `his`.

Next type

```shell
make install
# or 'make me a sandwich', if you feel so inclined
```

which puts his in your private bin directory (under $HOME). Or use

```shell
BINDIR=/what/ever make install
```

to install to `/what/ever`.

To remove artifacts, type

```shell
make clean
```

The code depends on `sqlite3`, and you need to have the headers and the
development lib available during compilation and linking. On debian-based
systems, you may need to `sudo apt-get install libsqlite-dev`. Whichever
distribution you use, make sure that `#include <sqlite3.h>` works and that
`-lsqlite3` finds `libsqlite3.a`. If you have different paths, then you might
need to adapt the Makefile.
