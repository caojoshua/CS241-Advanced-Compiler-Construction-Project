#
# Scanner.h
# Author: Joshua Cao
#

CC = g++
CFLAGS = -I include/ -I include/SSA/
EXTRA_CFLAGS = 

PUBLIC_TESTCASES = $(wildcard testcases/public/*.txt)
CUSTOM_TESTCASES = $(wildcard testcases/custom/*.txt)
ALL_TESTCASES = $(PUBLIC_TESTCASES) $(CUSTOM_TESTCASES)

SRCS = $(wildcard src/*.cpp) $(wildcard src/SSA/*.cpp)
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
	
run_separate_public : $(EXE)
	$(patsubst %, ./$(EXE) %;, $(PUBLIC_TESTCASES))

run_separate_custom : $(EXE)
	$(patsubst %, ./$(EXE) %;, $(CUSTOM_TESTCASES))
		
clean: $(EXE)
	rm $(EXE) graphml -r
