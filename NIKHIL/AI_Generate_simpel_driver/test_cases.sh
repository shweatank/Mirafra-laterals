#!/bin/bash

# Test cases for String Reversal Character Driver
# This script contains 10 different test cases

# Color codes for output
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Test case counter
TEST_COUNT=0
PASS_COUNT=0
FAIL_COUNT=0

# Define test cases as arrays: [description, input_string]
declare -a TEST_CASES=(
    "Default Test|Nikhil"
    "Single Character|A"
    "Two Characters|Hi"
    "String With Spaces|Hello World"
    "String With Numbers|Test123"
    "Palindrome|Level"
    "All Same Characters|AAAA"
    "Mixed Case|LiNuX"
    "Long String|CharacterDriver"
    "Numbers Only|1234567"
)

# Function to run a single test
run_test() {
    local test_name=$1
    local input_string=$2
    local expected_output=""
    local str_len=${#input_string}
    
    # Calculate expected reversed string manually
    for ((i = str_len - 1; i >= 0; i--)); do
        expected_output="${expected_output}${input_string:$i:1}"
    done
    
    ((TEST_COUNT++))
    
    echo -e "${BLUE}========================================${NC}"
    echo -e "${YELLOW}Test Case $TEST_COUNT: $test_name${NC}"
    echo -e "Input:    \"$input_string\""
    echo -e "Expected: \"$expected_output\""
    echo -e "${BLUE}========================================${NC}"
    
    # Run the user application and capture output
    output=$(./user_app "$input_string" 2>&1 | grep "Reversed String:")
    actual_reversed=$(echo "$output" | sed 's/.*Reversed String:  //')
    
    # Check if output matches expected
    if [ "$actual_reversed" = "$expected_output" ]; then
        echo -e "${GREEN}[PASS] Test Case $TEST_COUNT${NC}"
        ((PASS_COUNT++))
        result="PASS"
    else
        echo -e "${RED}[FAIL] Test Case $TEST_COUNT${NC}"
        echo -e "  Expected: \"$expected_output\""
        echo -e "  Got:      \"$actual_reversed\""
        ((FAIL_COUNT++))
        result="FAIL"
    fi
    
    # Additional output from application
    ./user_app "$input_string" 2>&1 | tail -3
    echo ""
}

# Function to check if driver is loaded
check_driver_loaded() {
    if lsmod | grep -q "reverse_driver"; then
        return 0
    else
        return 1
    fi
}

# Function to check if device exists
check_device_exists() {
    if [ -c /dev/reverse_char_dev ]; then
        return 0
    else
        return 1
    fi
}

# Main execution
main() {
    echo -e "${BLUE}╔════════════════════════════════════════════════════════════╗${NC}"
    echo -e "${BLUE}║   String Reversal Character Driver - Test Suite             ║${NC}"
    echo -e "${BLUE}╚════════════════════════════════════════════════════════════╝${NC}"
    echo ""
    
    # Check prerequisites
    echo -e "${YELLOW}[*] Checking prerequisites...${NC}"
    
    if [ ! -f "user_app" ]; then
        echo -e "${RED}[ERROR] user_app not found. Building...${NC}"
        make user_app
        if [ $? -ne 0 ]; then
            echo -e "${RED}[ERROR] Failed to build user_app${NC}"
            exit 1
        fi
    fi
    
    if ! check_driver_loaded; then
        echo -e "${YELLOW}[WARNING] Driver not loaded. Attempting to load...${NC}"
        sudo insmod reverse_driver.ko 2>/dev/null
        if [ $? -ne 0 ]; then
            echo -e "${RED}[ERROR] Failed to load driver. Please run:${NC}"
            echo -e "${RED}    make clean${NC}"
            echo -e "${RED}    make all${NC}"
            echo -e "${RED}    sudo make insmod${NC}"
            exit 1
        fi
    fi
    
    if ! check_device_exists; then
        echo -e "${RED}[ERROR] Device /dev/reverse_char_dev not found${NC}"
        echo -e "${RED}[ERROR] Please check if the driver is loaded correctly${NC}"
        exit 1
    fi
    
    echo -e "${GREEN}[OK] All prerequisites met${NC}"
    echo ""
    
    # Run all tests
    for test_case in "${TEST_CASES[@]}"; do
        IFS='|' read -r test_name test_input <<< "$test_case"
        run_test "$test_name" "$test_input"
    done
    
    # Summary
    echo -e "${BLUE}╔════════════════════════════════════════════════════════════╗${NC}"
    echo -e "${BLUE}║                        TEST SUMMARY                          ║${NC}"
    echo -e "${BLUE}╚════════════════════════════════════════════════════════════╝${NC}"
    echo -e "Total Tests:  $TEST_COUNT"
    echo -e "${GREEN}Passed:       $PASS_COUNT${NC}"
    echo -e "${RED}Failed:       $FAIL_COUNT${NC}"
    
    if [ $FAIL_COUNT -eq 0 ]; then
        echo -e "${GREEN}Result: ALL TESTS PASSED!${NC}"
        exit 0
    else
        echo -e "${RED}Result: SOME TESTS FAILED!${NC}"
        exit 1
    fi
}

# Run main function
main
