# Macros
CC = gcc217
# CC = gcc217m
CFLAGS =
# CFLAGS = -g
# CFLAGS = -D NDEBUG
# CFLAGS = -D NDEBUG -O

# Dependency rules for non-file targets
all: testsymtablelist testsymtablehash
clobber: clean
rm -f *~ \#*\#
clean:
rm -f testsymtable *.o

# Dependency rules for file targets
testsymtablelist: testsymtable.o symtablelist.o
$(CC) $(CFLAGS) testsymtable.o symtablelist.o -o testsymtablelist
testsymtablehash: testsymtable.o symtablehash.o
$(CC) $(CFLAGS) testsymtable.o symtablehash.o -o testsymtablehash

testsymtablelist.o: testsymtable.c symtablelist.c
$(CC) $(CFLAGS) -c testsymtable.c
testsymtablehash.o: testsymtable.c symtablehash.c
$(CC) $(CFLAGS) -c testsymtable.c

symtablelist.o: symtablelist.c symtable.h
$(CC) $(CFLAGS) -c symtablelist.c
symtablehash.o: symtablehash.c symtable.h
$(CC) $(CFLAGS) -c symtablehash.c