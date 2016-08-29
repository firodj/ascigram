CC = gcc
CFLAGS = -g -O3 -ansi -pedantic -Wall -Wextra -Wno-unused-parameter

ASCIGRAM_CFLAGS = $(CFLAGS) -Isrc
ifneq ($(OS),Windows_NT)
	ASCIGRAM_CFLAGS += -fPIC
endif

ASCIGRAM_SRC = \
	src\buffer.o \
	src\document.o \
	src\stack.o \
	src\svg.o \
	src\version.o

all: ascigram

# Executables

ascigram: bin\ascigram.o bin\option.o $(ASCIGRAM_SRC)
	$(CC) $^ $(LDFLAGS) -o $@

# Housekeeping

clean:
	del $(ASCIGRAM_SRC)
	del ascigram.exe


# Generic object compilations

%.o: %.c
	$(CC) $(ASCIGRAM_CFLAGS) -c -o $@ $<


# Testing

test: ascigram
	python test\runner.py
