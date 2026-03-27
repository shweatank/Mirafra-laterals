############################################################################
# Makefile for Linked List Project
# DATE: March 27, 2026
# DESC: Build system for compilation, running, and testing
############################################################################

# ========== VARIABLES ==========
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -pedantic -O2
DEBUGFLAGS = -g -O0 -DDEBUG
INCLUDE_DIR = include
SRC_DIR = src
TEST_DIR = tests
BUILD_DIR = build
LOGS_DIR = logs

# Source files
SOURCES = $(SRC_DIR)/linkedlist.c
OBJECTS = $(BUILD_DIR)/linkedlist.o
MAIN_SRC = $(SRC_DIR)/main.c
TEST_SRC = $(TEST_DIR)/test_linkedlist.c

# Output executables
MAIN_EXEC = $(BUILD_DIR)/main
TEST_EXEC = $(BUILD_DIR)/test_linkedlist

# ========== PHONY TARGETS ==========
.PHONY: all builddir run test clean help debug

# ========== DEFAULT TARGET ==========
all: $(MAIN_EXEC)
	@echo "✓ Build complete! Executable: $(MAIN_EXEC)"

# Create build directory
builddir:
	@mkdir -p $(BUILD_DIR)
	@echo "✓ Created build directory"

# Compile linkedlist.o
$(BUILD_DIR)/linkedlist.o: $(SOURCES) builddir
	@echo "→ Compiling linkedlist.c..."
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $(SOURCES) -o $@

# Link main executable
$(MAIN_EXEC): $(BUILD_DIR)/linkedlist.o $(MAIN_SRC)
	@echo "→ Linking main program..."
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) $(BUILD_DIR)/linkedlist.o $(MAIN_SRC) -o $@
	@echo "✓ Main executable created: $(MAIN_EXEC)"

# ========== RUN TARGET ==========
run: $(MAIN_EXEC)
	@echo ""
	@echo "╔════════════════════════════════════════════════╗"
	@echo "║     RUNNING LINKED LIST DEMO PROGRAM          ║"
	@echo "╚════════════════════════════════════════════════╝"
	@echo ""
	./$(MAIN_EXEC)

# ========== TEST TARGET ==========
test: $(TEST_EXEC)
	@echo ""
	@echo "╔════════════════════════════════════════════════╗"
	@echo "║     RUNNING COMPREHENSIVE TEST SUITE          ║"
	@echo "╚════════════════════════════════════════════════╝"
	@echo ""
	./$(TEST_EXEC)
	@echo ""
	@echo "✓ Test log saved to: $(LOGS_DIR)/testlog.txt"
	@echo ""

# Compile test executable
$(TEST_EXEC): $(BUILD_DIR)/linkedlist.o $(TEST_SRC) builddir
	@echo "→ Compiling test suite..."
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) $(BUILD_DIR)/linkedlist.o $(TEST_SRC) -o $@
	@echo "✓ Test executable created: $(TEST_EXEC)"

# ========== DEBUG TARGET ==========
debug: CFLAGS += $(DEBUGFLAGS)
debug: clean $(MAIN_EXEC)
	@echo "✓ Debug build complete!"

# ========== CLEAN TARGET ==========
clean:
	@echo "→ Cleaning build artifacts..."
	@rm -rf $(BUILD_DIR)
	@echo "✓ Build directory cleaned"
	@echo "✓ Object files removed"
	@echo "✓ Executables removed"

# ========== HELP TARGET ==========
help:
	@echo ""
	@echo "╔════════════════════════════════════════════════╗"
	@echo "║        LINKED LIST PROJECT - MAKEFILE        ║"
	@echo "╚════════════════════════════════════════════════╝"
	@echo ""
	@echo "Available targets:"
	@echo ""
	@echo "  make              - Compile project (default target)"
	@echo "  make run          - Build and run the demo program"
	@echo "  make test         - Build and run test suite"
	@echo "  make debug        - Build with debug symbols"
	@echo "  make clean        - Remove all build artifacts"
	@echo "  make help         - Display this help message"
	@echo ""
	@echo "Examples:"
	@echo "  make              # Just compile"
	@echo "  make run          # Compile and run demo"
	@echo "  make test         # Compile and run tests"
	@echo "  make clean        # Clean all build files"
	@echo ""

############################################################################
