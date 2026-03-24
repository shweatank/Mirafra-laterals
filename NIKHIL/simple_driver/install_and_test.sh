#!/bin/bash

# Comprehensive Installation and Testing Script
# This script automates the entire setup and testing process

set -e  # Exit on error

# Colors for output
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

PROJECT_DIR="/home/mirafra/test/git/Mirafra/AI_class/simple_driver"
DEVICE_PATH="/dev/reverse_char_dev"

# Function to print colored output
print_header() {
    echo -e "${BLUE}╔════════════════════════════════════════════════════════════╗${NC}"
    echo -e "${BLUE}║ $1${NC}"
    echo -e "${BLUE}╚════════════════════════════════════════════════════════════╝${NC}"
}

print_success() {
    echo -e "${GREEN}[✓] $1${NC}"
}

print_error() {
    echo -e "${RED}[✗] $1${NC}"
}

print_info() {
    echo -e "${YELLOW}[*] $1${NC}"
}

# Function to check if running as root
check_root() {
    if [ "$EUID" -ne 0 ]; then
        return 1
    fi
    return 0
}

# Function to check prerequisites
check_prerequisites() {
    print_header "Checking Prerequisites"
    
    # Check if we're in the right directory
    if [ ! -d "$PROJECT_DIR" ]; then
        print_error "Project directory not found: $PROJECT_DIR"
        exit 1
    fi
    cd "$PROJECT_DIR"
    print_success "Project directory found"
    
    # Check if kernel headers are installed
    if [ ! -d "/lib/modules/$(uname -r)/build" ]; then
        print_error "Kernel headers not found for kernel $(uname -r)"
        echo "Install with: sudo apt-get install linux-headers-$(uname -r)"
        exit 1
    fi
    print_success "Kernel headers installed"
    
    # Check for gcc
    if ! command -v gcc &> /dev/null; then
        print_error "GCC not found"
        exit 1
    fi
    print_success "GCC available"
    
    # Check for make
    if ! command -v make &> /dev/null; then
        print_error "Make not found"
        exit 1
    fi
    print_success "Make available"
    
    echo ""
}

# Function to build the project
build_project() {
    print_header "Building Project"
    
    print_info "Running: make clean && make all"
    if make clean && make all > /tmp/build.log 2>&1; then
        print_success "Build completed successfully"
        
        # Verify build artifacts
        if [ -f "reverse_driver.ko" ] && [ -f "user_app" ]; then
            print_success "Kernel module: $(ls -lh reverse_driver.ko | awk '{print $5}')"
            print_success "User app: $(ls -lh user_app | awk '{print $5}')"
        else
            print_error "Build artifacts not found"
            exit 1
        fi
    else
        print_error "Build failed. Check /tmp/build.log for details"
        cat /tmp/build.log
        exit 1
    fi
    echo ""
}

# Function to unload existing driver
unload_driver() {
    if lsmod | grep -q "reverse_driver"; then
        print_info "Unloading existing driver..."
        if rmmod reverse_driver 2>/dev/null; then
            print_success "Driver unloaded"
        else
            print_error "Failed to unload driver"
            return 1
        fi
    fi
    return 0
}

# Function to load the driver
load_driver() {
    print_header "Loading Kernel Module"
    
    # Unload existing driver first
    if ! unload_driver; then
        print_info "Continuing anyway..."
    fi
    
    sleep 1
    
    print_info "Loading reverse_driver.ko..."
    if insmod reverse_driver.ko 2>/dev/null; then
        print_success "Driver loaded successfully"
        sleep 1
        
        # Verify device was created
        if [ -c "$DEVICE_PATH" ]; then
            print_success "Device file created: $DEVICE_PATH"
            ls -l "$DEVICE_PATH"
        else
            print_error "Device file not found at $DEVICE_PATH"
            print_info "Trying to find it..."
            find /dev -name "*reverse*" 2>/dev/null || true
        fi
    else
        print_error "Failed to load driver"
        print_info "Check kernel messages: dmesg | tail -20"
        dmesg | tail -10
        exit 1
    fi
    echo ""
}

# Function to run a single test
run_test() {
    local test_name=$1
    local input_string=$2
    local expected=${3:-"auto"} # auto-calculate if not provided
    
    print_info "Test: $test_name"
    echo "  Input: \"$input_string\""
    
    # Run the user application
    output=$("$PROJECT_DIR/user_app" "$input_string" 2>&1)
    
    # Extract the reversed string
    reversed=$(echo "$output" | grep "Reversed String:" | sed 's/.*Reversed String:  //')
    
    # Extract result
    result=$(echo "$output" | grep "Result:" | head -1)
    
    if echo "$result" | grep -q "SUCCESS"; then
        echo "  $result"
        return 0
    else
        echo "  $result"
        return 1
    fi
}

# Function to run test suite
run_tests() {
    print_header "Running Test Suite"
    
    # Check if device exists and is accessible
    if [ ! -c "$DEVICE_PATH" ]; then
        print_error "Device not accessible at $DEVICE_PATH"
        print_info "Make sure driver is loaded: grep reverse_driver /proc/modules"
        return 1
    fi
    
    # Check permissions
    if [ ! -r "$DEVICE_PATH" ] || [ ! -w "$DEVICE_PATH" ]; then
        print_info "Fixing device permissions..."
        chmod 666 "$DEVICE_PATH" 2>/dev/null || true
    fi
    
    local pass_count=0
    local fail_count=0
    local test_num=0
    
    # Run tests
    declare -a tests=(
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
    
    for test_case in "${tests[@]}"; do
        IFS='|' read -r test_name test_input <<< "$test_case"
        ((test_num++))
        
        echo ""
        echo -e "${BLUE}[Test $test_num of ${#tests[@]}]${NC}"
        
        if run_test "$test_name" "$test_input"; then
            ((pass_count++))
            print_success "PASSED"
        else
            ((fail_count++))
            print_error "FAILED"
        fi
    done
    
    echo ""
    print_header "Test Summary"
    echo "Total Tests:  $test_num"
    echo -e "${GREEN}Passed:       $pass_count${NC}"
    echo -e "${RED}Failed:       $fail_count${NC}"
    
    if [ $fail_count -eq 0 ]; then
        echo -e "${GREEN}Result: ALL TESTS PASSED!${NC}"
        return 0
    else
        echo -e "${RED}Result: SOME TESTS FAILED!${NC}"
        return 1
    fi
}

# Function to show kernel messages
show_messages() {
    print_header "Kernel Messages"
    dmesg | grep "reverse_driver" | tail -20
}

# Function to cleanup
cleanup() {
    print_header "Cleanup"
    
    if check_root; then
        if lsmod | grep -q "reverse_driver"; then
            print_info "Unloading driver..."
            if rmmod reverse_driver 2>/dev/null; then
                print_success "Driver unloaded"
            fi
        fi
    fi
}

# Main execution
main() {
    if ! check_root; then
        print_error "This script requires root privileges"
        echo "Run with: sudo $0"
        exit 1
    fi
    
    echo ""
    print_header "String Reversal Character Driver - Complete Setup"
    echo ""
    
    # Run installation steps
    check_prerequisites
    build_project
    load_driver
    
    # Run tests
    if run_tests; then
        print_success "Installation and testing completed successfully!"
        exit 0
    else
        print_error "Some tests failed. Check above for details."
        show_messages
        exit 1
    fi
}

# Trap errors and cleanup
trap cleanup EXIT INT TERM

# Execute main
main
