CFLAGS=-g -Wall -W -std=c99
LDFLAGS=-L. -Wl,-R. -lbmem -static

C_SOURCES := $(wildcard *.c)
D_FILES := $(patsubst %.c,%.d,$(C_SOURCES))

.PHONY: clean all generate-deps
all: generate-deps libbmem.a libbmem.so main perf-test

generate-deps: $(D_FILES)

main: main.o
	$(CC) $^ -o $@ $(CFLAGS) $(LDFLAGS)

perf-test: perf-test.o
	$(CC) $^ -o $@ $(CFLAGS) $(LDFLAGS)

libbmem.so: bmem.c
	$(CC) -fPIC -shared $^ -o $@ $(CFLAGS)

libbmem.a: bmem.o
	$(AR) rcs $@ $^

%.d: %.c
	@set -e; rm -f $@; \
	$(CC) -MM $(CFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

include $(wildcard *.d)

clean:
	rm -f *.o *.d main perf-test libbmem.a libbmem.so
