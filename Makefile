BUILD_TYPE ?= release

CC = gcc

ifeq ($(BUILD_TYPE),release)
	CFLAGS = -DNDEBUG
else ifeq ($(BUILD_TYPE),debug)
	CFLAGS = -Wall -g -Wextra
endif

SRC_DIR = src
SRCMODULES = $(SRC_DIR)/encoder.c $(SRC_DIR)/arg.c $(SRC_DIR)/bmp.c
OBJMODULES = $(SRCMODULES:.c=.o)

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

enc2img: src/main.c $(OBJMODULES)
	$(CC) $(CFLAGS) $^ -o $@
