# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -std=c11 -pedantic -I/usr/include/x86_64-linux-gnu
# Linker flags
LDFLAGS = -pthread -lcurl

# Target executable name
TARGET = crawler

# Source files
SOURCES = crawler.c
# Object files
OBJECTS = $(SOURCES:.c=.o)

# Default target
all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean target
clean:
	rm -f $(OBJECTS) $(TARGET)

# Phony targets ensure that 'make' doesn’t get confused about actual files named after targets.
.PHONY: all clean

# Optional run target for convenience
run: $(TARGET)
	./$(TARGET)
