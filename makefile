# Entry point for the program
C_SRCS = main.c

# Compiler and flags
CC=gcc
CFLAGS=-std=c11 -Wall -Wextra

# Executable name
TARGET=simulation

OBJ=$(C_SRCS:.c=.o)

# usage: make / make all
all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# cleans previous builds
clean:
	rm -f $(TARGET) $(OBJ)