CC=gcc
CFLAGS=-Wall -Wextra -Wpedantic -std=c11
OUT_DIR=../bin
OBJ_DIR=../obj

example: main.c log.o
	$(CC) $(CFLAGS) -o $(OUT_DIR)/example main.c $(OBJ_DIR)/log.o

log.o: log.h log.c
	$(CC) $(CFLAGS) -o $(OBJ_DIR)/log.o -c log.c