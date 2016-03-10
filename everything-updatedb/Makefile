
CC  = gcc
LD  = gcc
CFLAGS = -Wall -g -c -I. -Ignulib/
LDFLAGS = -lsqlite3
TAR = everything-updatedb
SRC = $(wildcard *.c) 
OBJ = $(SRC:%.c=%.o)
GNUOBJ = gnulib/xmalloc.o gnulib/xalloc-die.o gnulib/exitfail.o gnulib/safe-read.o 

all: $(TAR)

$(TAR): $(OBJ) $(GNUOBJ)
	$(LD) $(OBJ) $(GNUOBJ) -o $(TAR) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@
	
clean:
	rm -f *.o $(TAR)
	make clean -C gnulib

.PHONY: all clean FORCE
