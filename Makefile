# # # # # # #
# Makefile for Assignment 1 comp30023
# Original: Matt Farrugia - Assignment 2 Design of Algorithms
# Modified by Kyle Zsembery - 911920
#

CC     = gcc
CFLAGS = -Wall -Wpedantic -std=c99 -g
# modify the flags here ^
EXE    = crack
OBJ    = pw_cracker.o sha256.o
# add any new object files here ^

# top (default) target
all: $(EXE)

# how to link executable
$(EXE): $(OBJ)
	$(CC) $(CFLAGS) -o $(EXE) $(OBJ)

# other dependencies, what headers are included by these objects, e.g.
pw_cracker.o: sha256.h
# hashtbl.o: strhash.h
# ^ add any new dependencies here (for example if you add new modules)


# phony targets (these targets do not represent actual files)
.PHONY: clean cleanly all CLEAN

# `make clean` to remove all object files
# `make CLEAN` to remove all object and executable files
# `make cleanly` to `make` then immediately remove object files (inefficient)
clean:
	rm -f $(OBJ)
CLEAN: clean
	rm -f $(EXE)
cleanly: all clean
