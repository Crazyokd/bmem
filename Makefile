all: bmem
CFLAGS=-g -Wall -W -std=c99

bmem: main.c bmem.c
	$(CC) main.c bmem.c -o bmem $(CFLAGS)

clean:
	rm -f bmem
