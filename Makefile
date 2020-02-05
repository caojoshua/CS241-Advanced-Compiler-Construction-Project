#
# Scanner.h
# Author: Joshua Cao
#

CC = g++
CFLAGS = -g -I include/

TESTCASES = $(wildcard testcases/*.txt)
SRCS = $(wildcard src/*.cpp)
DEPS = $(wildcard include/*.h)

EXE = compiler

all: $(DEPS) $(SRCS)
	$(CC) $(SRCS) -o $(EXE) $(CFLAGS)

runall: out
	./$(EXE) $(TESTCASES)

clean: $(EXE)
	rm $(EXE)