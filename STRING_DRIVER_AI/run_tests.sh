#!/bin/bash
#
# STRING_DRIVER_AI - Enhanced Test Runner
# Comprehensive test execution and reporting script
#

set -e

# Define paths
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BIN_DIR="$PROJECT_ROOT/bin"
DRIVER_BIN="$BIN_DIR/string_driver"
LOG_DIR="$PROJECT_ROOT/logs"
TEST_LOG="$LOG_DIR/test_results.log"
DETAILED_LOG="$LOG_DIR/test_detailed.log"

# Color codes
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Ensure directories exist
mkdir -p "$LOG_DIR"

print_header() {
    echo -e "${BLUE}=========================================${NC}"
    echo -e "${BLUE}$1${NC}"
    echo -e "${BLUE}=========================================${NC}"
}

print_success() {
    echo -e "${GREEN}✓ $1${NC}"
}

print_error() {
    echo -e "${RED}✗ $1${NC}"
}

print_info() {
    echo -e "${YELLOW}ℹ $1${NC}"
}

# Check if driver binary exists
if [ ! -f "$DRIVER_BIN" ]; then
    print_error "Test driver not found: $DRIVER_BIN"
    print_info "Please run 'make driver' first to build the test driver"
    exit 1
fi

# Create detailed log header
{
    echo "STRING_DRIVER_AI - Detailed Test Execution Log"
    echo "Generated: $(date '+%Y-%m-%d %H:%M:%S')"
    echo "========================================"
    echo ""
    echo "Environment:"
    echo "  System: $(uname -s)"
    echo "  Architecture: $(uname -m)"
    echo "  Kernel: $(uname -r)"
    echo "  Test Driver: $DRIVER_BIN"
    echo ""
    echo "========================================"
    echo ""
} > "$DETAILED_LOG"

print_header "Running STRING_DRIVER_AI Test Suite"
print_info "Starting automated test execution..."
echo ""

# Run the test driver with output capture
if cd "$BIN_DIR" && $DRIVER_BIN >> "$DETAILED_LOG" 2>&1; then
    DRIVER_EXIT_CODE=0
else
    DRIVER_EXIT_CODE=$?
fi

# Extract test results from detailed log
if [ -f "$PROJECT_ROOT/test_results.log" ]; then
    cat "$PROJECT_ROOT/test_results.log" >> "$DETAILED_LOG"
    mv "$PROJECT_ROOT/test_results.log" "$TEST_LOG"
elif [ -f "$BIN_DIR/test_results.log" ]; then
    cat "$BIN_DIR/test_results.log" >> "$DETAILED_LOG"
    mv "$BIN_DIR/test_results.log" "$TEST_LOG"
else
    print_error "No test results generated"
fi

# Parse and display results
if [ -f "$TEST_LOG" ]; then
    echo ""
    print_header "Test Results Summary"
    
    # Extract summary statistics
    TOTAL=$(grep -c "^\[" "$TEST_LOG" || echo "0")
    PASSED=$(grep -c "^\[PASS\]" "$TEST_LOG" || echo "0")
    FAILED=$(grep -c "^\[FAIL\]" "$TEST_LOG" || echo "0")
    
    if [ "$PASSED" -eq 0 ]; then
        PASSED=$(grep "Passed:" "$TEST_LOG" | tail -1 | awk '{print $NF}')
    fi
    
    if [ "$TOTAL" -eq 0 ]; then
        TOTAL=$(grep "Total Tests:" "$TEST_LOG" | tail -1 | awk '{print $NF}')
    fi
    
    if [ "$FAILED" -eq 0 ]; then
        FAILED=$(grep "Failed:" "$TEST_LOG" | tail -1 | awk '{print $NF}')
    fi
    
    # Display results
    echo ""
    print_success "Total Tests: $TOTAL"
    
    if [ "$PASSED" -gt 0 ]; then
        print_success "Passed: $PASSED"
    else
        print_error "Passed: $PASSED"
    fi
    
    if [ "$FAILED" -eq 0 ]; then
        print_success "Failed: $FAILED"
    else
        print_error "Failed: $FAILED"
    fi
    
    # Calculate success rate
    if [ "$TOTAL" -gt 0 ]; then
        SUCCESS_RATE=$((PASSED * 100 / TOTAL))
        if [ "$SUCCESS_RATE" -eq 100 ]; then
            print_success "Success Rate: ${SUCCESS_RATE}%"
        elif [ "$SUCCESS_RATE" -ge 80 ]; then
            print_info "Success Rate: ${SUCCESS_RATE}%"
        else
            print_error "Success Rate: ${SUCCESS_RATE}%"
        fi
    fi
    
    echo ""
fi

# Final status
print_header "Test Execution Complete"

if [ $DRIVER_EXIT_CODE -ne 0 ]; then
    print_error "Test driver exited with code: $DRIVER_EXIT_CODE"
    echo ""
    print_info "Logs saved to:"
    print_info "  - Test Results: $TEST_LOG"
    print_info "  - Detailed Log: $DETAILED_LOG"
    exit $DRIVER_EXIT_CODE
else
    print_success "All tests completed successfully!"
    echo ""
    print_info "Logs saved to:"
    print_success "  - Test Results: $TEST_LOG"
    print_success "  - Detailed Log: $DETAILED_LOG"
fi

echo ""
print_header "Next Steps"
print_info "View detailed results with: cat $TEST_LOG"
print_info "For full execution details: cat $DETAILED_LOG"
echo ""

exit 0
