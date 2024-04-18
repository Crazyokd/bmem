all: bmem test
CFLAGS=-g -Wall -W -std=c99

bmem: main.c bmem.c
	$(CC) main.c bmem.c -o bmem $(CFLAGS)

test: perf-test.c bmem.c
	$(CC) perf-test.c bmem.c -o perf-test $(CFLAGS)

clean:
	rm -f bmem
