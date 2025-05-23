# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = 

# Project files
MAIN_SRC = main.c
TEST_SRC = tests.c
HEADERS = main.h test.h
MAIN_OBJ = $(MAIN_SRC:.c=.o)
TEST_OBJ = $(TEST_SRC:.c=.o)
MAIN_EXEC = main.exe
TEST_EXEC = test.exe

# Default target
all: $(MAIN_EXEC) $(TEST_EXEC)

# Build main executable
$(MAIN_EXEC): $(MAIN_OBJ) $(TEST_OBJ)
	$(CC) $(CFLAGS) -o $@ $^

# Build test executable
$(TEST_EXEC): $(TEST_OBJ) $(MAIN_OBJ)
	$(CC) $(CFLAGS) -o $@ $^

# Compile source files into object files
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Run tests
test: $(TEST_EXEC)
	./$(TEST_EXEC)

# Run main program
run: $(MAIN_EXEC)
	./$(MAIN_EXEC)

# Clean build artifacts
clean:
	rm -f $(MAIN_OBJ) $(TEST_OBJ) $(MAIN_EXEC) $(TEST_EXEC)

# Phony targets
.PHONY: all test run clean
