all: bmem perf-test
CFLAGS=-g -Wall -W -std=c99

bmem: main.o bmem.o
	$(CC) $^ -o $@ $(CFLAGS)

perf-test: perf-test.o bmem.o
	$(CC) $^ -o $@ $(CFLAGS)

clean:
	rm -f bmem perf-test
