CFLAGS = -Wall -Wextra -pedantic -std=gnu11 -g
LDFLAGS =
CC = gcc

all: diskinfo disklist

diskinfo: diskinfo.o
	$(CC) $(CFLAGS) -o diskinfo diskinfo.o $(LDFLAGS)

disklist: disklist.o
	$(CC) $(CFLAGS) -o disklist disklist.o $(LDFLAGS)

clean:
	-rm -rf *.o *.dSYM diskinfo disklist
