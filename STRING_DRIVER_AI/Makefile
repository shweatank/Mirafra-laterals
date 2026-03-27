# Makefile for STRING_DRIVER_AI
# Production-Grade C11 String Operations Test Driver
# Standard: C11
# Platform: Linux x86_64 / arm64
# Build: Compiles with strict warnings and optimizations

CC := gcc
CFLAGS := -Wall -Wextra -pedantic -std=c11 -O2 -fPIC
CFLAGS += -D_DEFAULT_SOURCE -D_POSIX_C_SOURCE=200809L
LDFLAGS := -lm

# GD library for flowchart generation
GD_CFLAGS := $(shell pkg-config --cflags gdlib 2>/dev/null || echo "-I/usr/include")
GD_LIBS := $(shell pkg-config --libs gdlib 2>/dev/null || echo "-lgd -lpng -ljpeg -lz")

SRC_DIR := src
OBJ_DIR := build
BIN_DIR := bin
TEST_DIR := tests
LOG_DIR := logs

# Source files
MAIN_SRCS := $(SRC_DIR)/main.c $(SRC_DIR)/str_ops.c
DRIVER_SRCS := $(SRC_DIR)/driver.c $(SRC_DIR)/str_ops.c
FLOWCHART_SRCS := $(SRC_DIR)/flowchart.c

# Object files
MAIN_OBJS := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(MAIN_SRCS))
DRIVER_OBJS := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(DRIVER_SRCS))
FLOWCHART_OBJS := $(OBJ_DIR)/flowchart.o

# Executables
MAIN_BIN := $(BIN_DIR)/string_ops
DRIVER_BIN := $(BIN_DIR)/string_driver
FLOWCHART_BIN := $(BIN_DIR)/gen_flowchart

# Test script
TEST_SCRIPT := $(TEST_DIR)/test_cases.txt
RUN_TEST := run_tests.sh

# Default target
.PHONY: all driver clean test flowchart help

help:
	@echo "========================================="
	@echo "STRING_DRIVER_AI - Makefile targets"
	@echo "========================================="
	@echo "  make all       - Build all executables (main, driver, flowchart)"
	@echo "  make main      - Build interactive CLI only"
	@echo "  make driver    - Build automated test driver"
	@echo "  make flowchart - Build flowchart generator"
	@echo "  make test      - Run test driver and generate logs"
	@echo "  make clean     - Remove all build artifacts"
	@echo "  make help      - Show this help message"
	@echo "========================================="

all: $(MAIN_BIN) $(DRIVER_BIN) $(FLOWCHART_BIN)
	@echo "Build complete: $(MAIN_BIN), $(DRIVER_BIN), and $(FLOWCHART_BIN)"

main: $(MAIN_BIN)

driver: $(DRIVER_BIN)

flowchart: $(FLOWCHART_BIN)

# Main executable
$(MAIN_BIN): $(MAIN_OBJS) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "Built: $@"

# Test driver executable
$(DRIVER_BIN): $(DRIVER_OBJS) | $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "Built: $@"

# Flowchart generator executable
$(FLOWCHART_BIN): $(FLOWCHART_OBJS) | $(BIN_DIR)
	$(CC) $(CFLAGS) $(GD_CFLAGS) -o $@ $^ $(GD_LIBS) $(LDFLAGS)
	@echo "Built: $@"

# Main object files
$(OBJ_DIR)/main.o: $(SRC_DIR)/main.c $(SRC_DIR)/str_ops.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<
	@echo "Compiled: $@"

$(OBJ_DIR)/driver.o: $(SRC_DIR)/driver.c $(SRC_DIR)/str_ops.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<
	@echo "Compiled: $@"

$(OBJ_DIR)/str_ops.o: $(SRC_DIR)/str_ops.c $(SRC_DIR)/str_ops.h | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c -o $@ $<
	@echo "Compiled: $@"

# Flowchart object file
$(OBJ_DIR)/flowchart.o: $(SRC_DIR)/flowchart.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) $(GD_CFLAGS) -c -o $@ $<
	@echo "Compiled: $@"

# Create directories
$(OBJ_DIR):
	mkdir -p $@

$(BIN_DIR):
	mkdir -p $@

$(LOG_DIR):
	mkdir -p $@

# Generate flowchart
$(BIN_DIR)/flowchart.jpeg: $(FLOWCHART_BIN)
	cd $(BIN_DIR) && ./gen_flowchart
	@echo "Generated: $@"

# Run test driver and generate logs
test: $(DRIVER_BIN) | $(LOG_DIR)
	@echo "========================================="
	@echo "Running STRING_DRIVER_AI Test Suite"
	@echo "========================================="
	@cd $(BIN_DIR) && ./string_driver
	@mkdir -p ../logs
	@if [ -f test_results.log ]; then mv test_results.log ../logs/; fi
	@echo "Test logs saved to: logs/test_results.log"

# Run all (build and test)
run: all test

# Clean build artifacts
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)
	@echo "Cleaned build artifacts"

# Phony targets
.PHONY: all main driver flowchart test run clean help
