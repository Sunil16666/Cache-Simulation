# ---------------------------------------
# DISCLAIMER
# ---------------------------------------
# This Makefile is inspired by the Makefiles provided in past homework assignments.
# It has been adapted to work with both C++ and C
# ---------------------------------------

# Entry point for the program
C_SRCS = src/frontend/file_processing.c src/frontend/main.c
CPP_SRCS = src/simulation/primitiveGateCountCalc.cpp src/simulation/simulation.cpp # src/testing/testbench.cpp

# Compiler and flags
CC = gcc
CXX = g++
CFLAGS = -std=c17 -Wall -Wextra -g -D_POSIX_C_SOURCE=200809L $(INCLUDES)
CXXFLAGS = -std=c++14 -Wall -Wextra -g $(INCLUDES)

# SystemC path
INCLUDES = -I$(SYSTEMC_HOME)/include -Isrc/simulation -Isrc/frontend
# Detect the OS
UNAME := $(shell uname)

ifeq ($(UNAME), Darwin) # macOS
    LIBS = -L$(SYSTEMC_HOME)/lib -lsystemc -lm
else ifeq ($(OS), Windows_NT) # Windows
    LIBS = -L$(SYSTEMC_HOME)/lib -lsystemc -lm
else # Fallback / Linux
    LIBS = -L$(SYSTEMC_HOME)/lib-linux64 -lsystemc -lm
endif


# Executable name
TARGET = sc_main
testTARGET = test_main

C_OBJS = $(C_SRCS:.c=.o)
CPP_OBJS = $(CPP_SRCS:.cpp=.o)
OBJS = $(C_OBJS) $(CPP_OBJS)

# Determine if clang or gcc is available
CXX := $(shell command -v g++ || command -v clang++)
ifeq ($(strip $(CXX)),)
    $(error Neither clang++ nor g++ is available. Exiting.)
endif

# Add rpath except for MacOS
UNAME_S := $(shell uname -s)
ifneq ($(UNAME_S), Darwin)
    LDFLAGS += -Wl,-rpath=$(SYSTEMC_HOME)/lib
endif

# usage: make / make all
all: debug

# Debug build
debug: CFLAGS += -DDEBUG
debug: CXXFLAGS += -DDEBUG
debug: $(TARGET)

# Release build
release: CFLAGS += -O2
release: CXXFLAGS += -O2
release: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@ $(LIBS) $(LDFLAGS)

$(testTARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(CPP_OBJS) -o $@ $(LIBS) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# cleans previous builds
clean:
	rm -f $(TARGET) $(testTARGET) $(OBJS) *.vcd

.PHONY: all debug release clean
