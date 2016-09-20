CC = gcc
CFLAGS = -g -O3 -ansi -pedantic -Wall -Wextra -Wno-unused-parameter
WITHFLAGS = -D_WITH_STICKMAN -D_WITH_DBCYLINDER -D_WITH_DOCUMENTBOX
DEL = del

ASCIGRAM_CFLAGS = $(CFLAGS) -Isrc -Itest -Ibin -Isrc -Iutil
ifneq ($(OS),Windows_NT)
	ASCIGRAM_CFLAGS += -fPIC
	DEL = rm
endif

ASCIGRAM_SRC = \
	src/buffer.o \
	src/document.o \
	src/stack.o \
	src/svg.o \
	src/version.o \
	src/pattern.o \
	util/ansicolor-w32.o \
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
	$(DEL) $(ASCIGRAM_SRC)
	$(DEL) $(TEST_SRC)
	$(DEL) $(EXE_SRC)
	$(DEL) ascigram

# Generic object compilations

%.o: %.c
	$(CC) $(ASCIGRAM_CFLAGS) $(WITH_FLAGS) -c -o $@ $<


# Testing

test: testing
	./testing

pattern: ascigram
	python test\runner.py
