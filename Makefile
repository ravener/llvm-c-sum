CC = gcc
CCFLAGS = `llvm-config --cflags --ldflags --libs`
FILES = main.c

all:
	$(CC) $(CCFLAGS) $(FILES) -o sum
