CC = gcc
BASH_HEADERS := /usr/include/bash-plugins
CFLAGS := ${CFLAGS} $(shell pkg-config --cflags libpcre) -I${BASH_HEADERS} -std=c11
LDFLAGS := ${LDFLAGS} $(shell pkg-config --libs libpcre) -shared -fPIC

all: match.so

match.so: match.o
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(LDFLAGS) $(CFLAGS) -c -o $@ $<

.PHONY: clean
clean:
	rm -rf match.so match.o
