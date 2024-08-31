CC=gcc
CFLAGS=-Wall -Wextra -Wpedantic -std=c11
OUT_DIR=../bin
OBJ_DIR=../obj

log_example: main.c liblog.a
	$(CC) $(CFLAGS) main.c -L. -llog -o $(OUT_DIR)/log_example 

liblog.a: log.o
	ar -rcs liblog.a $(OBJ_DIR)/log.o

log.o: log.h log.c
	$(CC) $(CFLAGS) -o $(OBJ_DIR)/log.o -c log.c