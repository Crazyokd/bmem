CFLAGS=-g -Wall -W -std=c99
LDFLAGS=-L. -Wl,-R. -lbmem -static

all: libbmem.a libbmem.so main perf-test

main: main.o
	$(CC) $^ -o $@ $(CFLAGS) $(LDFLAGS)

perf-test: perf-test.o
	$(CC) $^ -o $@ $(CFLAGS) $(LDFLAGS)

libbmem.so: bmem.c
	$(CC) -fPIC -shared $^ -o $@ $(CFLAGS)

libbmem.a: bmem.o
	$(AR) rcs $@ $^

.PHONY: clean

clean:
	rm -f *.o main perf-test libbmem.a libbmem.so
