/**
 * linkedlist.h
 * 
 * Header file for a complete LinkedList implementation in C.
 * Contains struct definitions and function declarations for all operations.
 * 
 * All functions are documented with their purpose, parameters, and return values.
 * Memory is dynamically allocated and must be freed using destroyList().
 */

#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Node structure for the linked list.
 * 
 * Each node contains an integer data value and a pointer to the next node.
 * The last node's next pointer is NULL.
 */
typedef struct Node {
    int data;           /* Integer value stored in this node */
    struct Node* next;  /* Pointer to the next node in the list */
} Node;

/**
 * LinkedList structure to manage the list.
 * 
 * Maintains a head pointer to the first node and size count for efficiency.
 */
typedef struct LinkedList {
    Node* head;  /* Pointer to the first node in the list */
    int size;    /* Number of nodes currently in the list */
} LinkedList;

/*
 * ============================================================================
 *  LINKEDLIST CORE OPERATIONS
 * ============================================================================
 */

/**
 * createList - Initialize an empty linked list.
 * 
 * Returns: Pointer to a newly created empty LinkedList structure.
 *          Returns NULL if memory allocation fails.
 */
LinkedList* createList(void);

/**
 * destroyList - Free all memory allocated for the linked list.
 * 
 * Parameters:
 *   list - Pointer to the LinkedList to be destroyed
 * 
 * Returns: void
 * 
 * Note: After calling this function, the list pointer is invalid.
 *       Set it to NULL after calling destroyList(&list).
 */
void destroyList(LinkedList* list);

/*
 * ============================================================================
 *  INSERT OPERATIONS
 * ============================================================================
 */

/**
 * insertAtBeginning - Insert a node at the beginning of the list.
 * 
 * Parameters:
 *   list - Pointer to the LinkedList
 *   data - Integer value to insert
 * 
 * Returns: 1 on success, 0 on failure (memory allocation error)
 */
int insertAtBeginning(LinkedList* list, int data);

/**
 * insertAtEnd - Insert a node at the end of the list.
 * 
 * Parameters:
 *   list - Pointer to the LinkedList
 *   data - Integer value to insert
 * 
 * Returns: 1 on success, 0 on failure (memory allocation error)
 */
int insertAtEnd(LinkedList* list, int data);

/**
 * insertAtPosition - Insert a node at a specific position (0-indexed).
 * 
 * Parameters:
 *   list     - Pointer to the LinkedList
 *   data     - Integer value to insert
 *   position - Index where the node should be inserted (0 = beginning)
 * 
 * Returns: 1 on success, 0 on failure (invalid position or memory error)
 * 
 * Note: If position > size, insertion will fail. Position must be <= size.
 */
int insertAtPosition(LinkedList* list, int data, int position);

/*
 * ============================================================================
 *  DELETE OPERATIONS
 * ============================================================================
 */

/**
 * deleteAtBeginning - Remove the first node from the list.
 * 
 * Parameters:
 *   list - Pointer to the LinkedList
 * 
 * Returns: 1 on success, 0 on failure (list is empty)
 */
int deleteAtBeginning(LinkedList* list);

/**
 * deleteAtEnd - Remove the last node from the list.
 * 
 * Parameters:
 *   list - Pointer to the LinkedList
 * 
 * Returns: 1 on success, 0 on failure (list is empty)
 */
int deleteAtEnd(LinkedList* list);

/**
 * deleteAtPosition - Remove the node at a specific position.
 * 
 * Parameters:
 *   list     - Pointer to the LinkedList
 *   position - Index of the node to remove (0-indexed)
 * 
 * Returns: 1 on success, 0 on failure (invalid position or empty list)
 */
int deleteAtPosition(LinkedList* list, int position);

/**
 * deleteByValue - Find and remove the first node with the given value.
 * 
 * Parameters:
 *   list  - Pointer to the LinkedList
 *   value - The integer value to search for and delete
 * 
 * Returns: 1 if value found and deleted, 0 if value not found
 */
int deleteByValue(LinkedList* list, int value);

/*
 * ============================================================================
 *  SEARCH AND ACCESS OPERATIONS
 * ============================================================================
 */

/**
 * search - Find the index of a node with the given value.
 * 
 * Parameters:
 *   list  - Pointer to the LinkedList
 *   value - The integer value to search for
 * 
 * Returns: Index of the first matching node (0-indexed), or -1 if not found
 */
int search(LinkedList* list, int value);

/**
 * getLength - Return the number of nodes in the list.
 * 
 * Parameters:
 *   list - Pointer to the LinkedList
 * 
 * Returns: Number of nodes in the list (0 for empty list)
 */
int getLength(LinkedList* list);

/**
 * findMiddle - Return a pointer to the middle node of the list.
 * 
 * Parameters:
 *   list - Pointer to the LinkedList
 * 
 * Returns: Pointer to the middle node, or NULL if list is empty
 * 
 * Note: For even-length lists, returns the first node of the second half.
 *       Uses slow and fast pointer technique for O(n) efficiency.
 */
Node* findMiddle(LinkedList* list);

/**
 * nthFromEnd - Get the nth node from the end of the list (1-indexed).
 * 
 * Parameters:
 *   list - Pointer to the LinkedList
 *   n    - Position from end (1 = last node, 2 = second to last, etc.)
 * 
 * Returns: Pointer to the nth node from end, or NULL if n is invalid
 */
Node* nthFromEnd(LinkedList* list, int n);

/*
 * ============================================================================
 *  DISPLAY AND UTILITY OPERATIONS
 * ============================================================================
 */

/**
 * printList - Print all node values in the list.
 * 
 * Parameters:
 *   list - Pointer to the LinkedList
 * 
 * Returns: void
 * 
 * Prints: [val1] -> [val2] -> [val3] -> NULL
 */
void printList(LinkedList* list);

/*
 * ============================================================================
 *  SORTING AND REORDERING OPERATIONS
 * ============================================================================
 */

/**
 * reverseList - Reverse the order of all nodes in the list in-place.
 * 
 * Parameters:
 *   list - Pointer to the LinkedList
 * 
 * Returns: void
 * 
 * Note: Reversal is done in-place without creating new nodes.
 */
void reverseList(LinkedList* list);

/**
 * sortList - Sort the list in ascending order using bubble sort.
 * 
 * Parameters:
 *   list - Pointer to the LinkedList
 * 
 * Returns: void
 * 
 * Note: Bubble sort is used for simplicity. Time complexity: O(n^2)
 */
void sortList(LinkedList* list);

/**
 * mergeSort - Sort the list in ascending order using merge sort.
 * 
 * Parameters:
 *   list - Pointer to the LinkedList
 * 
 * Returns: void
 * 
 * Note: Time complexity: O(n log n). More efficient than bubble sort.
 */
void mergeSort(LinkedList* list);

/**
 * mergeTwoLists - Merge two sorted linked lists into one sorted list.
 * 
 * Parameters:
 *   list1 - Pointer to the first sorted LinkedList
 *   list2 - Pointer to the second sorted LinkedList
 * 
 * Returns: Pointer to a new LinkedList containing merged nodes from both lists.
 *          Returns NULL if memory allocation fails.
 * 
 * Note: Creates new nodes; original lists remain unchanged.
 *       Both input lists must be sorted in ascending order.
 */
LinkedList* mergeTwoLists(LinkedList* list1, LinkedList* list2);

/*
 * ============================================================================
 *  ADVANCED OPERATIONS
 * ============================================================================
 */

/**
 * removeDuplicates - Remove all duplicate values from the list.
 * 
 * Parameters:
 *   list - Pointer to the LinkedList
 * 
 * Returns: void
 * 
 * Note: Keeps the first occurrence of each value, removes subsequent duplicates.
 *       For efficiency, sort the list first before calling this function.
 */
void removeDuplicates(LinkedList* list);

/**
 * detectLoop - Detect if there is a cycle in the linked list.
 * 
 * Parameters:
 *   list - Pointer to the LinkedList
 * 
 * Returns: 1 if a cycle is detected, 0 if the list is acyclic
 * 
 * Note: Uses Floyd's cycle detection algorithm (slow and fast pointers).
 *       Time complexity: O(n), Space complexity: O(1)
 */
int detectLoop(LinkedList* list);

#endif /* LINKEDLIST_H */
