# Makefile for his

# Where does a default 'make install' put the binary.
# Alternatively use: BINDIR=/what/ever make install
BINDIR = $$HOME/bin

VER = '1.00'
AUTHOR = 'Karel Kubat <karel@kubat.nl>'
YEARS = '2017ff'
URL = 'http://www.kubat.nl/pages/his'

SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)

# Top level target, creates the binary 'his' in current directory
local: usagetxt.h his
	@echo
	@echo 'Type "make install" if you want to install his.'
	@echo 'Type "BINDIR=/what/ever make install" to change the path.'

install: local
	install -s his $(BINDIR)
	@echo
	@echo 'Installation successful, $(BINDIR)/his can be used'

# Loadtest some random stuff
loadtest: local
	install -s his /tmp
	rm -f /tmp/loadtest.out
	perl loadtest.pl | tee /tmp/loadtest.out
	@echo
	@echo "Output is also in /tmp/loadtest.out"

# Linking the objects
his: $(OBJ)
	$(CC) -g -Wall -o $@ $(OBJ) -lsqlite3

# Compilation rule
%.o: %.c
	$(CC) -g -c -Wall -o $@ \
	  -DVER=\"$(VER)\" -DAUTHOR=\"$(AUTHOR)\" -DYEARS=\"$(YEARS)\" \
	  -DURL=\"$(URL)\" $<

# How to generate .h from text files
usagetxt.h: usagetxt.txt Makefile
	perl txt2h.pl $< $@ USAGETEXT 0
formatstxt.h: formatstxt.txt
	perl txt2h.pl $< $@ FORMATSTEXT 0
createtablestxt.h: createtablestxt.txt
	perl txt2h.pl $< $@ CREATETABLESTEXT 0
readmetxt.h: README.txt
	perl txt2h.pl $< $@ READMETEXT 1

# Clean up
clean:
	rm -f his $(OBJ) usagetxt.h formatstxt.h createtablestxt.h

# Extra deps, due to generated .h files from .txt and data instantiation
main.o: main.c his.h
usage.o: usage.c usagetxt.h
listformats.o: listformats.c formatstxt.h
sqlinit.o: sqlrun.c createtablestxt.h
readme.o: readme.c readmetxt.h
