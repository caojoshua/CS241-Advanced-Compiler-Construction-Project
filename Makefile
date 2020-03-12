#
# Scanner.h
# Author: Joshua Cao
#

CC = g++
CFLAGS = -I include/
EXTRA_CFLAGS = 

ALL_TESTCASES = $(wildcard testcases/*.txt)
PUBLIC_TESTCASES = $(wildcard testcases/public/*.txt)
CUSTOM_TESTCASES = $(wildcard testcases/custom/*.txt)

SRCS = $(wildcard src/*.cpp)
DEPS = $(wildcard include/*.h)

EXE = compiler

all: $(DEPS) $(SRCS)
	$(CC) $(SRCS) -o $(EXE) $(CFLAGS) $(EXTRA_CFLAGS)
	
all_debug_symbols: $(DEPS) $(SRCS)
	$(MAKE) all EXTRA_CFLAGS=-g

run_all: $(EXE)
	./$(EXE) $(ALL_TESTCASES)

run_public: $(EXE)
	./$(EXE) $(PUBLIC_TESTCASES)
	
run_custom : $(EXE)
	./$(EXE) $(CUSTOM_TESTCASES)
	
clean: $(EXE)
	rm $(EXE) graphml -r