# Makefile for his

# --------------------------------------------------------------------------
# Settings
# --------------------------------------------------------------------------

# Where does a default 'make install' put the binary.
# Alternatively use: BINDIR=/what/ever make install
BINDIR ?= $$HOME/bin

# Versioning.
# 1.00 [2017-09-01] First version
# 1.01 [2017-09-08] Bugfix in listcmd.c 
# 1.02 [2017-09-10] SQL string comparison in listcmd.c defaults to '=' unless
#                   %/_ wildcards  are used, then the comparison becomes 'LIKE'
# 1.03 [KK 2017-10-04] Added db locked retries
# 1.04 [KK 2017-10-04] BINDIR=... make install honors the bindir
# 1.05 [KK 2018-07-16] Added support for $HISTIGNORE

VER    = '1.05'
AUTHOR = 'Karel Kubat <karel@kubat.nl>'
YEARS  = '2017ff'
URL    = 'https://github.com/KarelKubat/his'

SRC    = $(wildcard *.c)
OBJ    = $(SRC:.c=.o)

# --------------------------------------------------------------------------
# Top level rules
# --------------------------------------------------------------------------

# No-op target to show help
foo:
	@cat COMPILING.txt
	exit 1

# Creates the binary 'his' in current directory
his: $(OBJ)
	$(CC) -g -Wall -o $@ $(OBJ) -lsqlite3

# Installs to $(BINDIR)
install: his
	mkdir -p $(BINDIR)
	install -s his $(BINDIR)
	@echo
	@echo 'Installation successful, $(BINDIR)/his can be used'

me:
	@true
a:
	@true
sandwich: install

# Loadtest some random stuff
loadtest: his
	install -s his /tmp
	rm -f /tmp/loadtest.out
	perl loadtest.pl | tee /tmp/loadtest.out
	@echo
	@echo "Output is also in /tmp/loadtest.out"

upload: clean
	(cd ..; tar czf /tmp/his.tar.gz his/)
	scp /tmp/his.tar.gz www-data@kb:~/www.kubat.nl/www/

# Clean up
clean:
	rm -f his $(OBJ) usagetxt.h formatstxt.h createtablestxt.h

# --------------------------------------------------------------------------
# Helper rules
# --------------------------------------------------------------------------

# Compilation rule
%.o: %.c
	$(CC) -g -c -Wall -o $@ \
	  -DVER=\"$(VER)\" -DAUTHOR=\"$(AUTHOR)\" -DYEARS=\"$(YEARS)\" \
	  -DURL=\"$(URL)\" $<

# How to generate .h from text files
usagetxt.h: usagetxt.txt Makefile
	perl txt2h.pl $< $@ USAGETEXT 0
formatstxt.h: formatstxt.txt Makefile
	perl txt2h.pl $< $@ FORMATSTEXT 1
createtablestxt.h: createtablestxt.txt Makefile
	perl txt2h.pl $< $@ CREATETABLESTEXT 0

# Extra deps, due to generated .h files from .txt and data instantiation
main.o: main.c his.h
usage.o: usage.c usagetxt.h
listformats.o: listformats.c formatstxt.h
sqlinit.o: sqlrun.c createtablestxt.h
readme.o: readme.c readmetxt.h
