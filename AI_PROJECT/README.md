# LinkedList in C

A complete, production-grade LinkedList implementation in C with full build system, comprehensive tests, and documentation.

## Overview

This project provides a fully functional LinkedList data structure with 20 essential operations. The implementation follows best practices in systems programming with:

- **Memory safety**: All allocated memory is properly freed
- **Error handling**: All functions return appropriate error codes
- **Documentation**: Every function is documented with purpose, parameters, and return values
- **Testing**: Comprehensive test suite covering all operations and edge cases
- **Build system**: Professional Makefile with multiple targets
- **Code quality**: Compiled with `-Wall -Wextra -g` flags

## Project Structure

```
AI_PROJECT/
├── src/
│   ├── linkedlist.h        # Header with struct and function declarations
│   └── linkedlist.c        # All LinkedList operation implementations
├── test/
│   ├── test_linkedlist.c   # Comprehensive test suite
│   └── test_runner.h       # Test framework macros
├── build/                  # Compiled objects and binaries (auto-created)
├── logs/
│   └── test_results.log    # Test output log (auto-generated)
├── Makefile                # Build system
└── README.md               # This file
```

## Operations Implemented

All 20 operations have been fully implemented and tested:

| Operation | Description | Time Complexity | Space Complexity |
|-----------|-------------|-----------------|------------------|
| `createList()` | Initialize an empty linked list | O(1) | O(1) |
| `destroyList()` | Free all allocated memory | O(n) | O(1) |
| `insertAtBeginning()` | Insert node at head | O(1) | O(1) |
| `insertAtEnd()` | Insert node at tail | O(n) | O(1) |
| `insertAtPosition()` | Insert at specific index | O(n) | O(1) |
| `deleteAtBeginning()` | Remove head node | O(1) | O(1) |
| `deleteAtEnd()` | Remove tail node | O(n) | O(1) |
| `deleteAtPosition()` | Remove node at index | O(n) | O(1) |
| `deleteByValue()` | Find and remove first match | O(n) | O(1) |
| `search()` | Find index of value | O(n) | O(1) |
| `getLength()` | Return number of nodes | O(1) | O(1) |
| `printList()` | Print all node values | O(n) | O(1) |
| `reverseList()` | Reverse list in-place | O(n) | O(1) |
| `sortList()` | Sort using bubble sort | O(n²) | O(1) |
| `mergeSort()` | Sort using merge sort | O(n log n) | O(n) |
| `mergeTwoLists()` | Merge two sorted lists | O(n + m) | O(n + m) |
| `findMiddle()` | Find middle node (slow/fast pointer) | O(n) | O(1) |
| `nthFromEnd()` | Get nth node from end | O(n) | O(1) |
| `removeDuplicates()` | Remove duplicate values | O(n) | O(1) |
| `detectLoop()` | Floyd's cycle detection | O(n) | O(1) |

## Data Structures

### Node Structure
```c
typedef struct Node {
    int data;           /* Integer value stored in node */
    struct Node* next;  /* Pointer to next node */
} Node;
```

### LinkedList Structure
```c
typedef struct LinkedList {
    Node* head;  /* Pointer to first node */
    int size;    /* Number of nodes in list */
} LinkedList;
```

## Build Instructions

### Prerequisites
- GCC compiler (version 4.8 or later)
- GNU Make
- Optional: Valgrind (for memory checking)

### Compile and Run Tests
```bash
# Build and run tests (default)
make

# Compile only (creates object files in build/)
make build

# Run tests only
make test

# Run test binary directly
make run

# Memory leak check with valgrind
make memcheck

# Clean build artifacts
make clean

# Display build information
make info

# Show help
make help
```

### Manual Compilation
```bash
# Compile source only
gcc -Wall -Wextra -g -c src/linkedlist.c -o build/linkedlist.o

# Compile and run tests
gcc -Wall -Wextra -g -I src -o build/test_linkedlist test/test_linkedlist.c src/linkedlist.c
./build/test_linkedlist
```

## Test Results

Tests are located in `test/test_linkedlist.c` and cover:

- **Insert operations**: beginning, end, position
- **Delete operations**: beginning, end, position, by value
- **Search operations**: value location, length
- **Access operations**: middle node, nth from end
- **Sorting operations**: bubble sort, merge sort
- **Advanced operations**: reverse, merge two lists, remove duplicates, loop detection
- **Edge cases**: empty list, single node, invalid operations

When you run `make test`, results are:
1. Printed to the terminal in real-time
2. Saved to `logs/test_results.log` with timestamp

### Test Log Format
```
============================
Test Run: 2025-03-27 14:30:00
============================
[PASS] test_insertAtBeginning
[PASS] test_insertAtEnd
[PASS] test_insertAtPosition
...
[PASS] test_detectLoop
============================
Results: 45 passed, 0 failed
============================
```

## Example Usage

### Create and Use a LinkedList
```c
#include "src/linkedlist.h"
#include <stdio.h>

int main(void) {
    /* Create an empty list */
    LinkedList* list = createList();
    
    /* Insert elements */
    insertAtEnd(list, 10);
    insertAtEnd(list, 20);
    insertAtEnd(list, 30);
    insertAtBeginning(list, 5);
    
    /* Print the list */
    printList(list);  /* Output: [5] -> [10] -> [20] -> [30] -> NULL */
    
    /* Search for a value */
    int index = search(list, 20);
    printf("Found 20 at index: %d\n", index);  /* Output: 2 */
    
    /* Get list length */
    int len = getLength(list);
    printf("List length: %d\n", len);  /* Output: 4 */
    
    /* Reverse the list */
    reverseList(list);
    printList(list);  /* Output: [30] -> [20] -> [10] -> [5] -> NULL */
    
    /* Sort the list */
    sortList(list);
    printList(list);  /* Output: [5] -> [10] -> [20] -> [30] -> NULL */
    
    /* Clean up */
    destroyList(list);
    
    return 0;
}
```

### Compile and Run Example
```bash
# Save the above code as main.c, then:
gcc -Wall -Wextra -g -I src -o main main.c src/linkedlist.c
./main
```

## Memory Management

All memory allocated with `malloc()` is properly freed:

- `createList()` allocates list structure
- `insertAtBeginning()`, `insertAtEnd()`, `insertAtPosition()` allocate nodes
- `deleteAtBeginning()`, `deleteAtEnd()`, `deleteAtPosition()`, `deleteByValue()` free nodes
- `destroyList()` frees all nodes and the list structure

**Important**: Always call `destroyList()` to avoid memory leaks:
```c
LinkedList* list = createList();
// ... use list ...
destroyList(list);  /* Must be called */
list = NULL;        /* Good practice */
```

## Compiler Flags

The project uses production-quality compiler flags:

```makefile
CFLAGS = -Wall -Wextra -g
```

- `-Wall`: Enable all common warnings
- `-Wextra`: Enable extra warnings beyond `-Wall`
- `-g`: Include debug symbols for GDB

## Code Standards

- **Language**: C99 standard
- **Style**: Clear variable names, comprehensive comments
- **Functions**: All have documentation blocks explaining purpose, parameters, and return values
- **Error handling**: Functions return error codes where appropriate (0 for failure, 1 for success)

## Performance Characteristics

### Best Case Scenarios
- Insert/delete at beginning: O(1)
- Search: O(1) if element is first
- Get length: O(1)

### Worst Case Scenarios
- Insert/delete at end: O(n)
- Search: O(n) if element is last
- Reverse/Sort: O(n) or O(n²) depending on operation

### Space Complexity
- Each node: ~16 bytes (int + pointer)
- List structure: ~16 bytes
- Total for n elements: O(n) nodes

## Troubleshooting

### Build Issues
```bash
# Clean and rebuild
make clean && make

# Check compiler
gcc --version

# Verbose compilation
make clean test CFLAGS="-Wall -Wextra -g -v"
```

### Memory Issues
```bash
# Check for leaks
make memcheck

# If valgrind not installed, install it:
# Ubuntu/Debian:
sudo apt-get install valgrind

# CentOS/RHEL:
sudo yum install valgrind

# macOS:
brew install valgrind
```

### Test Failures
1. Check test output: `cat logs/test_results.log`
2. Run specific test: `./build/test_linkedlist`
3. Compile with debug info: `make CFLAGS="-Wall -Wextra -g"`
4. Use GDB: `gdb ./build/test_linkedlist`

## Implementation Notes

### Slow and Fast Pointer Technique
Used in `findMiddle()` and `detectLoop()` for efficient traversal without storing length.

### Bubble Sort vs Merge Sort
- `sortList()` uses bubble sort: simple, O(n²), good for small lists
- `mergeSort()` uses merge sort: complex, O(n log n), good for large lists

### Memory Safety
- All malloc() calls checked for NULL
- All pointers validated before dereferencing
- Double-free prevented by clearing pointers after free

## Author

LinkedList Implementation in C - Production Grade
Created: March 2025
Standards: C99, POSIX

## License

This is a complete educational and professional implementation.
Feel free to use, modify, and distribute.

---

**For more information or issues**, refer to the inline code comments in `src/linkedlist.c`.
