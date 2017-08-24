# Makefile for his

VER = '1.00'
AUTHOR = 'Karel Kubat <karel@kubat.nl>'
YEARS = '2017ff'
URL = 'http://www.kubat.nl/pages/his'

SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)

# Top level target, creates the binary 'his' in current directory
foo: usagetxt.h his

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
	perl txt2h.pl $< $@ USAGETEXT
formatstxt.h: formatstxt.txt
	perl txt2h.pl $< $@ FORMATSTEXT
createtablestxt.h: createtablestxt.txt
	perl txt2h.pl $< $@ CREATETABLESTEXT

# Clean up
clean:
	rm -f his $(OBJ) usagetxt.h formatstxt.h createtablestxt.h

# Extra deps, due to generated .h files from .txt and data instantiation
main.o: main.c his.h
usage.o: usage.c usagetxt.h
listformats.o: listformats.c formatstxt.h
sqlinit.o: sqlrun.c createtablestxt.h
