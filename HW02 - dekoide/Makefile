CC=gcc
CFLAGS=-Wall -Wextra -Wpedantic -std=c11
OUT_DIR=../bin
OBJ_DIR=../obj

dekoide: dekoide.c non-ascii.o
	$(CC) $(CFLAGS) -o $(OUT_DIR)/dekoide dekoide.c $(OBJ_DIR)/non-ascii.o

non-ascii.o: non-ascii.h non-ascii.c
	$(CC) $(CFLAGS) -o $(OBJ_DIR)/non-ascii.o -c non-ascii.c