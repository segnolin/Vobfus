CC = gcc
CFLAGS = -no-pie -fno-stack-protector

all: cal hash sort

cal: cal.c
	$(CC) $(CFLAGS) -o $@ $^

hash: hash.c
	$(CC) $(CFLAGS) -o $@ $^

sort: sort.c
	$(CC) $(CFLAGS) -o $@ $^
