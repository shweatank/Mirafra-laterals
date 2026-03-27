#!/bin/bash

# Simple Driver Troubleshooting and Setup Script
# Run this to quickly load the driver and test it

set -e

PROJECT_DIR="/home/mirafra/test/git/Mirafra/AI_class/simple_driver"
DEVICE_PATH="/dev/reverse_char_dev"

echo "════════════════════════════════════════════════════════════"
echo "String Reversal Character Driver - Setup Guide"
echo "════════════════════════════════════════════════════════════"
echo ""

# Step 1: Verify we're in the right directory
if [ ! -f "reverse_driver.ko" ]; then
    echo "[ERROR] reverse_driver.ko not found!"
    echo "Make sure you're in: $PROJECT_DIR"
    exit 1
fi

# Step 2: Check if driver is already loaded
if lsmod | grep -q "reverse_driver"; then
    echo "[✓] Driver is ALREADY LOADED"
    echo ""
else
    echo "[!] Driver is NOT loaded yet"
    echo ""
    echo "To load the driver, run:"
    echo "    sudo insmod reverse_driver.ko"
    echo ""
    echo "After loading, verify with:"
    echo "    lsmod | grep reverse_driver"
    echo "    ls -l /dev/reverse_char_dev"
    echo ""
    exit 2
fi

# Step 3: Check if device file exists
if [ -c "$DEVICE_PATH" ]; then
    echo "[✓] Device file exists at $DEVICE_PATH"
    ls -l "$DEVICE_PATH"
    echo ""
else
    echo "[!] Device file NOT found at $DEVICE_PATH"
    exit 2
fi

# Step 4: Try running user_app
echo "[*] Testing with default input 'Nikhil'..."
echo ""
if ./user_app; then
    echo ""
    echo "[✓✓✓] SUCCESS! Driver is working correctly!"
else
    echo ""
    echo "[✗] Test failed. Check device permissions:"
    echo "    sudo chmod 666 /dev/reverse_char_dev"
fi

echo ""
echo "════════════════════════════════════════════════════════════"
