# Entry point for the program
C_SRCS = main.c

# Compiler and flags
CC=gcc
CXX = g++
CFLAGS = -std=c17 -Wall -Wextra
CXXFLAGS = -std=c++14 -Wall -Wextra

# SystemC path
SYSC_PATH = / # Use Environment vars instead
INCLUDES = -I$(SYSTEMC)/include
LIBS = -L$(SYSTEMC)/lib -lsystemc

# Executable name
TARGET = simulation

OBJ = $(C_SRCS:.c=.o)

# usage: make / make all
all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# cleans previous builds
clean:
	rm -f $(TARGET) $(OBJ)