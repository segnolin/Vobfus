CC = gcc
CFLAGS = -no-pie -fno-stack-protector

all: cal hash

cal: cal.c
	$(CC) $(CFLAGS) -o $@ $^

hash: hash.c
	$(CC) $(CFLAGS) -o $@ $^
