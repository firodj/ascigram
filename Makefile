CC = gcc
CFLAGS = -g -O3 -ansi -pedantic -Wall -Wextra -Wno-unused-parameter

ASCIGRAM_CFLAGS = $(CFLAGS) -Isrc -Itest -Ibin -Isrc/patterns -Iutil
ifneq ($(OS),Windows_NT)
	ASCIGRAM_CFLAGS += -fPIC
endif

ASCIGRAM_SRC = \
	src/buffer.o \
	src/document.o \
	src/stack.o \
	src/svg.o \
	src/version.o \
	src/pattern.o \
	util/ansicolor-w32.o
	src/patterns/patterns.o \
	src/patterns/stickman.o \
	src/patterns/dbcylinder.o \
	src/patterns/documentbox.o

TEST_SRC = \
	test/test.o \
	test/test_stack.o \
	test/test_document.o \
	test/test_pattern.o

EXE_SRC = \
	bin/ascigram.o \
	bin/option.o

all: ascigram

# Executables

ascigram: $(EXE_SRC) $(ASCIGRAM_SRC)
	$(CC) $^ $(LDFLAGS) -o $@

testing: $(TEST_SRC) $(ASCIGRAM_SRC)
	$(CC) $^ $(LDFLAGS) -o $@

# Housekeeping

clean:
	rm $(ASCIGRAM_SRC)
	rm $(TEST_SRC)
	rm $(EXE_SRC)
	rm ascigram

# Generic object compilations

%.o: %.c
	$(CC) $(ASCIGRAM_CFLAGS) -c -o $@ $<


# Testing

test: testing
	testing

pattern: ascigram
	python test\runner.py
