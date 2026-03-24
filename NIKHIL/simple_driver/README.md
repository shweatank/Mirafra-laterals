# String Reversal Character Driver

A simple Linux character device driver that reverses strings received from user space.

## Project Structure

- **reverse_driver.c**: Kernel character driver that handles string reversal
- **user_app.c**: User space application to communicate with the driver
- **Makefile**: Build configuration for driver and application
- **test_cases.sh**: Automated test suite with 10 test cases
- **README.md**: This file

## Features

- Character device driver (`/dev/reverse_char_dev`)
- Send strings character-by-character from user space
- Reverse the string in kernel space
- Return reversed string to user space
- Full error handling and logging
- 10 comprehensive test cases

## Prerequisites

- Linux kernel headers
- GCC compiler
- Make utility
- Root access (for loading/unloading driver)

### Install on Ubuntu/Debian:
```bash
sudo apt-get install linux-headers-$(uname -r)
sudo apt-get install build-essential
```

## Building

### Build everything:
```bash
make all
```

Or build separately:
```bash
make module      # Build kernel module only
make user_app    # Build user space app only
```

## Loading the Driver

```bash
sudo make insmod
```

Verify the driver is loaded:
```bash
lsmod | grep reverse_driver
ls -l /dev/reverse_char_dev
```

## Usage

### Default (reverse "Nikhil"):
```bash
./user_app
```

### Reverse a custom string:
```bash
./user_app "Hello"
./user_app "Linux"
./user_app "CharacterDriver"
```

### Example Output:
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

## Running Tests

### Run automated test suite:
```bash
sudo chmod +x test_cases.sh
./test_cases.sh
```

### Test Cases Included:

1. **Default Test**: "Nikhil" → "lihkilN"
2. **Single Character**: "A" → "A"
3. **Two Characters**: "Hi" → "iH"
4. **String With Spaces**: "Hello World" → "dlroW olleH"
5. **String With Numbers**: "Test123" → "321tseT"
6. **Palindrome**: "Level" → "leveL"
7. **All Same Characters**: "AAAA" → "AAAA"
8. **Mixed Case**: "LiNuX" → "XuNiL"
9. **Long String**: "CharacterDriver" → "revirDretarahC"
10. **Numbers Only**: "1234567" → "7654321"

## Kernel Messages

View kernel messages from the driver:
```bash
make dmesg
```

Or manually:
```bash
sudo dmesg | tail -20
```

## Unloading the Driver

```bash
sudo make rmmod
```

## Cleaning Up

Remove all build artifacts:
```bash
make clean
```

## Architecture

### Driver Flow:
1. Module initialization registers character device
2. User app opens `/dev/reverse_char_dev`
3. User app writes string character-by-character
4. Driver stores characters in kernel buffer
5. User app reads from device
6. Driver reverses the string and returns it to user space
7. User app closes device

### File Operations Implemented:
- `open()` - Opens device file
- `write()` - Receives data from user space
- `read()` - Sends reversed string to user space
- `release()` - Closes device file

## Troubleshooting

### Device file not created:
```bash
sudo udevadm control --reload-rules
sudo udevadm trigger
```

### Permission denied when running user_app:
```bash
sudo chown root:root /dev/reverse_char_dev
sudo chmod 666 /dev/reverse_char_dev
```

### Module fails to load:
```bash
make clean
make all
sudo make rmmod   # Remove old version if exists
sudo make insmod
```

### Build errors regarding kernel headers:
```bash
sudo apt-get install linux-headers-$(uname -r)
make clean
make all
```

## Limitations

- Maximum buffer size: 256 bytes
- String must be null-terminated
- Kernel module requires root access
- Not recursive - each read/write transaction must complete before next

## Author

**Nikhil**

Linux Character Device Driver Project

## License

GPL v2
