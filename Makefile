# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -std=c11 -pedantic -pthread -I/usr/include/libxml2 $(shell xml2-config --cflags)

# Linker flags
LDFLAGS = -lcurl $(shell xml2-config --libs)

# Target executable name
TARGET = crawler

# Source files
SOURCES = crawler.c
# Object files
OBJECTS = $(SOURCES:.c=.o)

# Default build target
all: $(TARGET)

# Link the program
$(TARGET): $(OBJECTS)
    $(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# Compile source files into object files
%.o: %.c
    $(CC) $(CFLAGS) -c $< -o $@

# Clean up binary and object files
clean:
    rm -f $(OBJECTS) $(TARGET)

# Run the program
run: $(TARGET)
    ./$(TARGET)

# Ensure these aren't considered as filenames
.PHONY: all clean run
