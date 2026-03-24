# Driver Setup & Troubleshooting Guide

## Problem: Device Not Found

**Error Message:**
```
Failed to open device. Make sure the driver is loaded and device exists: 
No such file or directory
Device path: /dev/reverse_char_dev
```

**Root Cause:** The kernel module hasn't been loaded yet, so the device file doesn't exist.

---

## Solution: Load the Driver

The driver needs to be loaded with `sudo` (requires administrator privileges).

### Step 1: Navigate to Project Directory
```bash
cd /home/mirafra/test/git/Mirafra/AI_class/simple_driver
```

### Step 2: Load the Kernel Module
```bash
sudo insmod reverse_driver.ko
```

You'll be prompted for your password. Enter it and press Enter.

**Output should be:**
```
[sudo] password for mirafra: ••••••••
```
(No output means success)

### Step 3: Verify Driver is Loaded
```bash
lsmod | grep reverse_driver
```

**Expected output:**
```
reverse_driver        16384  0
```

### Step 4: Check Device File
```bash
ls -l /dev/reverse_char_dev
```

**Expected output:**
```
crw-rw-rw- 1 root root 250, 0 Mar 24 17:15 /dev/reverse_char_dev
```

### Step 5: Test the Driver
```bash
./user_app "Nikhil"
```

**Expected output:**
```
User Space Application: String Reversal
=========================================
Input String: Nikhil
Input Length: 6 bytes

Writing string to device character by character...
  Wrote: 'N' (index 0)
  Wrote: 'i' (index 1)
  Wrote: 'k' (index 2)
  Wrote: 'h' (index 3)
  Wrote: 'i' (index 4)
  Wrote: 'l' (index 5)

Reading reversed string from device...

=========================================
Original String:  Nikhil
Reversed String:  lihkilN
Bytes Reversed:   6
=========================================
Result: SUCCESS - String correctly reversed!
```

---

## Alternative: Use Makefile

Instead of manual commands, you can use the Makefile:

```bash
# Build and load driver (all-in-one)
make clean && make all && sudo make insmod

# Test immediately
./user_app

# Or test with custom input
./user_app "Hello"

# View kernel messages
make dmesg

# Unload when finished
sudo make rmmod
```

---

## Quick Commands Summary

| Action | Command |
|--------|---------|
| **Load driver** | `sudo insmod reverse_driver.ko` |
| **Verify load** | `lsmod \| grep reverse_driver` |
| **Check device** | `ls -l /dev/reverse_char_dev` |
| **Test default** | `./user_app` |
| **Test custom** | `./user_app "your_string"` |
| **Run all tests** | `./test_cases.sh` |
| **Unload driver** | `sudo rmmod reverse_driver` |
| **View kernel logs** | `sudo dmesg \| tail -20` |

---

## If Device Has Wrong Permissions

If you get "Permission denied," fix permissions:

```bash
sudo chmod 666 /dev/reverse_char_dev
```

---

## If Driver Won't Load

Try rebuilding:

```bash
make clean
make all
sudo make insmod
```

---

## If Nothing Works

Check kernel messages for detailed error information:

```bash
sudo dmesg | grep -i reverse
```

This will show what went wrong when loading the driver.

---

## Complete Example Session

```bash
# Step 1: Navigate
cd /home/mirafra/test/git/Mirafra/AI_class/simple_driver

# Step 2: Ensure it's built
make all

# Step 3: Load (you'll enter password here)
sudo insmod reverse_driver.ko

# Step 4: Verify
lsmod | grep reverse_driver

# Step 5: Test
./user_app "Nikhil"

# Step 6: Run all tests
./test_cases.sh

# Step 7: Cleanup (when done)
sudo rmmod reverse_driver
```

---

## Notes

- You need to be a sudoer (your account can run `sudo` commands)
- Once loaded, the driver stays in memory until you run `sudo rmmod reverse_driver`
- The device file `/dev/reverse_char_dev` is automatically created when the driver loads
- The driver is automatically removed when unloaded with `sudo rmmod`

**Status: Ready to test!** 🚀

Just load the driver and you're good to go.
