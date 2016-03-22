CFLAGS = -Wall -Wextra -pedantic -std=gnu11 -g
LDFLAGS =
CC = gcc

all: diskinfo

diskinfo: diskinfo.o
	$(CC) $(CFLAGS) -o diskinfo diskinfo.o $(LDFLAGS)

clean:
	-rm -rf *.o diskinfo.dSYM diskinfo
