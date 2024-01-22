CC=gcc
CFLAGS=-Wall -Wextra -Wpedantic -std=c11
OUT_DIR=../bin
OBJ_DIR=../obj

wttr: wttr.c 
	$(CC) $(CFLAGS) -o $(OUT_DIR)/wttr wttr.c ./cJSON/cJSON.c -lcurl