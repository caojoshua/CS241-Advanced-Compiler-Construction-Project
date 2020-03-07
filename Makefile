#
# Scanner.h
# Author: Joshua Cao
#

CC = g++
CFLAGS = -g -I include/

ALL_TESTCASES = $(wildcard testcases/*.txt)
PUBLIC_TESTCASES = $(wildcard testcases/public/*.txt)
CUSTOM_TESTCASES = $(wildcard testcases/custom/*.txt)

SRCS = $(wildcard src/*.cpp)
DEPS = $(wildcard include/*.h)

EXE = compiler

all: $(DEPS) $(SRCS)
	$(CC) $(SRCS) -o $(EXE) $(CFLAGS)

run_all: $(EXE)
	./$(EXE) $(ALL_TESTCASES)

run_public: $(EXE)
	./$(EXE) $(PUBLIC_TESTCASES)
	
run_custom : $(EXE)
	./$(EXE) $(CUSTOM_TESTCASES)
	
clean: $(EXE)
	rm $(EXE) graphml -r