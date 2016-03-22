CFLAGS = -Wall -Wextra -pedantic -std=gnu11 -g
LDFLAGS =
CC = gcc

all: diskinfo disklist diskget

diskinfo: diskinfo.o
	$(CC) $(CFLAGS) -o diskinfo diskinfo.o $(LDFLAGS)

disklist: disklist.o
	$(CC) $(CFLAGS) -o disklist disklist.o $(LDFLAGS)

diskget: diskget.o
	$(CC) $(CFLAGS) -o diskget diskget.o $(LDFLAGS)

clean:
	-rm -rf *.o *.dSYM diskinfo disklist diskget
