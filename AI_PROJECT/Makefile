# LinkedList Makefile
# Complete build system for LinkedList project

CC       = gcc
CFLAGS   = -Wall -Wextra -g
SRCDIR   = src
TESTDIR  = test
BUILDDIR = build
LOGSDIR  = logs

SRC_FILES  = $(SRCDIR)/linkedlist.c
TEST_FILES = $(TESTDIR)/test_linkedlist.c
TEST_BIN   = $(BUILDDIR)/test_linkedlist
RESULTS    = $(LOGSDIR)/test_results.log

# ============================================================================
# PHONY TARGETS (not file-based)
# ============================================================================
.PHONY: all build test run clean memcheck info help

# Default: test everything
all: test

# Build directories and compile source
build: 
	mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -c $(SRCDIR)/linkedlist.c -o $(BUILDDIR)/linkedlist.o

# Compile and run tests
test:
	@mkdir -p $(BUILDDIR) $(LOGSDIR)
	@echo "Compiling test suite..."
	@$(CC) $(CFLAGS) -I$(SRCDIR) -o $(TEST_BIN) $(TEST_FILES) $(SRC_FILES)
	@echo "Running tests..."
	@echo "============================" > $(RESULTS)
	@echo "Test Run: $$(date '+%Y-%m-%d %H:%M:%S')" >> $(RESULTS)
	@echo "============================" >> $(RESULTS)
	@$(TEST_BIN) | tee -a $(RESULTS)
	@echo "============================" >> $(RESULTS)
	@echo "Test run complete. Results saved to $(RESULTS)"

# Run test binary directly
run: test
	@$(TEST_BIN)

# Memory check with valgrind
memcheck: test
	@command -v valgrind >/dev/null 2>&1 || { echo "Valgrind not installed. Skipping memory check."; exit 0; }
	@echo "Running memory check with valgrind..."
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose $(TEST_BIN)

# Clean build artifacts and logs
clean:
	@echo "Cleaning build artifacts..."
	@rm -rf $(BUILDDIR)
	@rm -f $(RESULTS)
	@echo "Clean complete."

# Display build information
info:
	@echo "LinkedList Build Configuration"
	@echo "=============================="
	@echo "Compiler:     $(CC)"
	@echo "CFLAGS:       $(CFLAGS)"
	@echo "Source dir:   $(SRCDIR)"
	@echo "Build dir:    $(BUILDDIR)"
	@echo "Test dir:     $(TESTDIR)"
	@echo "Logs dir:     $(LOGSDIR)"

# Show help
help:
	@echo "LinkedList Makefile Targets"
	@echo "=============================="
	@echo "  all        - Build and run tests (default)"
	@echo "  build      - Compile source to object files"
	@echo "  test       - Compile and run tests with logging"
	@echo "  run        - Run test binary"
	@echo "  clean      - Remove build artifacts"
	@echo "  memcheck   - Run tests under valgrind"
	@echo "  info       - Display build information"
	@echo "  help       - Show this help message"
	@echo ""
	@echo "Example usage:"
	@echo "  make              # Build and test"
	@echo "  make clean test   # Clean and rebuild tests"
	@echo "  make memcheck     # Check for memory leaks"
