# Macros
CC = gcc217
# CC = gcc217m
CFLAGS =
# CFLAGS = -g
# CFLAGS = -D NDEBUG
# CFLAGS = -D NDEBUG -O

# Dependency rules for non-file targets
all: testsymtable
clobber: clean
rm -f *~ \#*\#
clean:
rm -f testsymtable *.o

# Dependency rules for file targets
testintmath: testintmath.o intmath.o
$(CC) $(CFLAGS) testintmath.o intmath.o -o testintmath
testintmath.o: testintmath.c intmath.h
$(CC) $(CFLAGS) -c testintmath.c
intmath.o: intmath.c intmath.h
$(CC) $(CFLAGS) -c intmath.c