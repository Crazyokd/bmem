CFLAGS=-g -Wall -W -std=c99
LDFLAGS=-L. -Wl,-R. -lbmem -static

.PHONY: clean all
all: libbmem.a libbmem.so main perf-test

main: main.o
	$(CC) $^ -o $@ $(CFLAGS) $(LDFLAGS)

perf-test: perf-test.o
	$(CC) $^ -o $@ $(CFLAGS) $(LDFLAGS)

libbmem.so: bmem.c bmem.h
	$(CC) -fPIC -shared $^ -o $@ $(CFLAGS)

libbmem.a: bmem.o bmem.h
	$(AR) rcs $@ $^

clean:
	rm -f *.o main perf-test libbmem.a libbmem.so
