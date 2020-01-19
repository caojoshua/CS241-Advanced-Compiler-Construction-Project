
SDIR = src/
IDIR = include/

TESTCASES = $(wildcard testcases/*.txt)

CC = g++
CFLAGS = -g -I $(IDIR)

_SRCS = main.cpp Scanner.cpp
SRCS = $(patsubst %, $(SDIR)%, $(_SRCS))

_DEPS = Scanner.h
DEPS = $(patsubst %, $(IDIR)%, $(_DEPS))

make: $(DEPS) $(SRCS)
	$(CC) $(SRCS) -o out $(CFLAGS)

runall: out
	./out $(TESTCASES)

clean: out
	rm out