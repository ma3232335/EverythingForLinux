
CC  = gcc
LD  = gcc
CFLAGS = -Wall -g -c -I..
SRC = $(wildcard *.c) 
OBJ = $(SRC:%.c=%.o)

all: $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f *.o 

.PHONY:
	all clean
