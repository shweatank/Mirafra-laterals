#!/bin/bash

# Quick Start Guide for String Reversal Character Driver
# This script provides step-by-step instructions to set up and run the driver

echo "=========================================="
echo "String Reversal Character Driver"
echo "Quick Start Guide"
echo "=========================================="
echo ""

# Step 1: Display file structure
echo "[Step 1] Project Files Created:"
echo "=========================================="
ls -lh *.c *.ko *.sh Makefile README.md 2>/dev/null | awk '{print $9, "(" $5 ")"}'
echo ""

# Step 2: Build instructions
cat << 'EOF'
[Step 2] Build Instructions:
==========================================
To compile the driver and user application:

    cd /home/mirafra/test/git/Mirafra/AI_class/simple_driver
    make clean
    make all

Expected output:
    - reverse_driver.ko (kernel module)
    - user_app (user space application)

EOF

# Step 3: Load instructions
cat << 'EOF'
[Step 3] Load the Driver (Requires ROOT):
==========================================
To load the kernel module into the system:

    sudo make insmod

This will:
    - Load reverse_driver.ko
    - Create device file /dev/reverse_char_dev
    - Initialize the character driver

Verify driver is loaded:
    lsmod | grep reverse_driver
    ls -l /dev/reverse_char_dev

EOF

# Step 4: Test instructions
cat << 'EOF'
[Step 4] Run User Space Application:
==========================================
Test with default string "Nikhil":

    ./user_app

Test with custom string:

    ./user_app "Hello"
    ./user_app "Linux"
    ./user_app "CharacterDriver"

Expected output format:
    Input String:  [your string]
    Reversed String:  [reversed string]
    Result: SUCCESS - String correctly reversed!

EOF

# Step 5: Automated tests
cat << 'EOF'
[Step 5] Run Automated Test Suite:
==========================================
To run all 10 test cases automatically:

    sudo chmod +x test_cases.sh
    ./test_cases.sh

Test Cases:
    1. Default Test - "Nikhil" → "lihkilN"
    2. Single Character - "A" → "A"
    3. Two Characters - "Hi" → "iH"
    4. String With Spaces - "Hello World" → "dlroW olleH"
    5. String With Numbers - "Test123" → "321tseT"
    6. Palindrome - "Level" → "leveL"
    7. All Same Characters - "AAAA" → "AAAA"
    8. Mixed Case - "LiNuX" → "XuNiL"
    9. Long String - "CharacterDriver" → "revirDretarahC"
    10. Numbers Only - "1234567" → "7654321"

EOF

# Step 6: View kernel messages
cat << 'EOF'
[Step 6] View Kernel Messages:
==========================================
To see kernel module debug messages:

    sudo dmesg | tail -20

Or use the Makefile:

    make dmesg

This shows driver initialization and operation messages.

EOF

# Step 7: Unload driver
cat << 'EOF'
[Step 7] Unload the Driver:
==========================================
To remove the driver from the system:

    sudo make rmmod

This will:
    - Unload the kernel module
    - Remove device file /dev/reverse_char_dev
    - Clean up kernel resources

EOF

# Step 8: Additional Makefile targets
cat << 'EOF'
[Step 8] Additional Makefile Targets:
==========================================
    make all           - Build kernel module and user app (default)
    make module        - Build kernel module only
    make user_app      - Build user space application only
    make insmod        - Load kernel module (requires sudo)
    make rmmod         - Unload kernel module (requires sudo)
    make run           - Build and run user app with default 'Nikhil'
    make run_custom    - Run with custom input (e.g., make run_custom INPUT="hello")
    make dmesg         - Show kernel messages
    make clean         - Remove build artifacts
    make help          - Show help message

EOF

# Display project structure
echo ""
echo "[Project Structure]"
echo "=========================================="
echo "simple_driver/"
echo "├── reverse_driver.c     # Kernel module (string reversal logic)"
echo "├── user_app.c           # User space app (calls the driver)"
echo "├── Makefile             # Build configuration"
echo "├── test_cases.sh        # Automated test suite (10 test cases)"
echo "├── README.md            # Full documentation"
echo "└── QUICKSTART.sh        # This file"
echo ""
echo "[Ready to Use!]"
echo "=========================================="
echo "All files have been successfully created!"
echo "Follow the steps above to build, load, and test the driver."
echo ""
EOF
