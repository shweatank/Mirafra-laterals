/*
 * ============================================================================
 * FILE: linkedlist.h
 * DESC: Header file for Linked List implementation with all operations
 * AUTH: System Programmer
 * DATE: March 27, 2026
 * ============================================================================
 */

#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* Node structure definition */
typedef struct Node {
    int data;                    /* Integer data stored in node */
    struct Node *next;           /* Pointer to next node */
} Node;

/* Linked List structure */
typedef struct LinkedList {
    Node *head;                  /* Pointer to first node */
    int count;                   /* Number of nodes in list */
} LinkedList;

/* ========== BASIC OPERATIONS ========== */

/**
 * ll_create_list()
 * DESC: Creates and initializes an empty linked list
 * RETURN: Pointer to newly created LinkedList, NULL on failure
 */
LinkedList* ll_create_list(void);

/**
 * ll_create_node()
 * DESC: Creates a new node with given data
 * PARAM: data - integer value to store
 * RETURN: Pointer to new Node, NULL on failure
 */
Node* ll_create_node(int data);

/* ========== INSERTION OPERATIONS ========== */

/**
 * ll_insert_beginning()
 * DESC: Inserts node at the beginning of the list
 * PARAM: list - pointer to LinkedList, data - value to insert
 * RETURN: 0 on success, -1 on failure
 */
int ll_insert_beginning(LinkedList *list, int data);

/**
 * ll_insert_end()
 * DESC: Inserts node at the end of the list
 * PARAM: list - pointer to LinkedList, data - value to insert
 * RETURN: 0 on success, -1 on failure
 */
int ll_insert_end(LinkedList *list, int data);

/**
 * ll_insert_at_position()
 * DESC: Inserts node at specific position (1-indexed)
 * PARAM: list - pointer to LinkedList, position - where to insert, data - value
 * RETURN: 0 on success, -1 on failure
 */
int ll_insert_at_position(LinkedList *list, int position, int data);

/* ========== DELETION OPERATIONS ========== */

/**
 * ll_delete_beginning()
 * DESC: Deletes first node from the list
 * PARAM: list - pointer to LinkedList
 * RETURN: Deleted node's data on success, -1 on failure
 */
int ll_delete_beginning(LinkedList *list);

/**
 * ll_delete_end()
 * DESC: Deletes last node from the list
 * PARAM: list - pointer to LinkedList
 * RETURN: Deleted node's data on success, -1 on failure
 */
int ll_delete_end(LinkedList *list);

/**
 * ll_delete_at_position()
 * DESC: Deletes node at specific position (1-indexed)
 * PARAM: list - pointer to LinkedList, position - which node to delete
 * RETURN: Deleted node's data on success, -1 on failure
 */
int ll_delete_at_position(LinkedList *list, int position);

/* ========== TRAVERSAL & DISPLAY ========== */

/**
 * ll_display()
 * DESC: Displays all nodes in the list
 * PARAM: list - pointer to LinkedList
 * RETURN: void
 */
void ll_display(LinkedList *list);

/**
 * ll_display_reverse()
 * DESC: Displays list in reverse order (recursive)
 * PARAM: node - current node to display
 * RETURN: void
 */
void ll_display_reverse(Node *node);

/* ========== SEARCH & COUNT ========== */

/**
 * ll_search()
 * DESC: Searches for element in the list
 * PARAM: list - pointer to LinkedList, data - value to search
 * RETURN: Position (1-indexed) if found, -1 if not found
 */
int ll_search(LinkedList *list, int data);

/**
 * ll_count()
 * DESC: Returns the total number of nodes in the list
 * PARAM: list - pointer to LinkedList
 * RETURN: Count of nodes
 */
int ll_count(LinkedList *list);

/* ========== LIST MANIPULATION ========== */

/**
 * ll_reverse()
 * DESC: Reverses the entire linked list
 * PARAM: list - pointer to LinkedList
 * RETURN: 0 on success, -1 on failure
 */
int ll_reverse(LinkedList *list);

/**
 * ll_sort()
 * DESC: Sorts the list in ascending order (bubble sort)
 * PARAM: list - pointer to LinkedList
 * RETURN: 0 on success, -1 on failure
 */
int ll_sort(LinkedList *list);

/**
 * ll_detect_loop()
 * DESC: Detects if there is a loop in the list (Floyd's Cycle Detection)
 * PARAM: list - pointer to LinkedList
 * RETURN: 1 if loop exists, 0 if no loop, -1 on error
 */
int ll_detect_loop(LinkedList *list);

/* ========== MEMORY MANAGEMENT ========== */

/**
 * ll_clear()
 * DESC: Deletes all nodes and clears the list
 * PARAM: list - pointer to LinkedList
 * RETURN: void
 */
void ll_clear(LinkedList *list);

/**
 * ll_free_list()
 * DESC: Frees the linked list structure completely
 * PARAM: list - pointer to LinkedList
 * RETURN: void
 */
void ll_free_list(LinkedList **list);

/* ========== UTILITY FUNCTIONS ========== */

/**
 * ll_is_empty()
 * DESC: Checks if the list is empty
 * PARAM: list - pointer to LinkedList
 * RETURN: 1 if empty, 0 if not empty
 */
int ll_is_empty(LinkedList *list);

/**
 * ll_get_middle()
 * DESC: Finds and returns the middle element of the list
 * PARAM: list - pointer to LinkedList
 * RETURN: Middle element value, -1 if list is empty
 */
int ll_get_middle(LinkedList *list);

#endif /* LINKEDLIST_H */
