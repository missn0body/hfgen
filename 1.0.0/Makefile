# Simply replace CC with your own compiler
# if it doesn't match the ones listed below.

#CC = i586-pc-msdosdjgpp-g[cc/++]
#CC = i686-w64-mingw32-g[cc/++]
#CC = x86_64-w64-mingw32-g[cc/++]
CC = gcc
RM = rm
CFLAGS = -Wall -Wextra -Wpedantic -std=c2x -g -Ofast

.PHONY: all clean
all: hfgen

clean:
	$(RM) -f *.o hfgen

hfgen: hfgen.o
	$(CC) $^ -o $@ $(CFLAGS)

hfgen.o: hfgen.c
	$(CC) $^ -o $@ $(CFLAGS) -r
