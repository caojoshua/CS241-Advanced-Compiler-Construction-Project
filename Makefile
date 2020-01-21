#
# Scanner.h
# Author: Joshua Cao
#

CC = g++
CFLAGS = -g -I include/

TESTCASES = $(wildcard testcases/*.txt)
SRCS = $(wildcard src/*.cpp)
DEPS = $(wildcard include/*.h)

all: $(DEPS) $(SRCS)
	$(CC) $(SRCS) -o out $(CFLAGS)

runall: out
	./out $(TESTCASES)

clean: out
	rm out