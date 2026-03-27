/*
 * ============================================================================
 * FILE: linkedlist.c
 * DESC: Implementation of all linked list operations
 * AUTH: System Programmer
 * DATE: March 27, 2026
 * ============================================================================
 */

#include "../include/linkedlist.h"

/* ========== BASIC OPERATIONS ========== */

/**
 * ll_create_list()
 * Creates and initializes an empty linked list with head=NULL, count=0
 */
LinkedList* ll_create_list(void) {
    LinkedList *list = (LinkedList*)malloc(sizeof(LinkedList));
    if (list == NULL) {
        fprintf(stderr, "ERROR: Memory allocation failed for LinkedList\n");
        return NULL;
    }
    
    list->head = NULL;
    list->count = 0;
    return list;
}

/**
 * ll_create_node()
 * Creates a new node with given data and initializes next pointer to NULL
 */
Node* ll_create_node(int data) {
    Node *newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL) {
        fprintf(stderr, "ERROR: Memory allocation failed for Node\n");
        return NULL;
    }
    
    newNode->data = data;
    newNode->next = NULL;
    return newNode;
}

/* ========== INSERTION OPERATIONS ========== */

/**
 * ll_insert_beginning()
 * Inserts a new node at the beginning of the list
 * Updates head pointer and increments count
 */
int ll_insert_beginning(LinkedList *list, int data) {
    if (list == NULL) {
        fprintf(stderr, "ERROR: List is NULL\n");
        return -1;
    }
    
    Node *newNode = ll_create_node(data);
    if (newNode == NULL) {
        return -1;
    }
    
    newNode->next = list->head;
    list->head = newNode;
    list->count++;
    
    return 0;
}

/**
 * ll_insert_end()
 * Inserts a new node at the end of the list
 * Traverses to last node and updates its next pointer
 */
int ll_insert_end(LinkedList *list, int data) {
    if (list == NULL) {
        fprintf(stderr, "ERROR: List is NULL\n");
        return -1;
    }
    
    Node *newNode = ll_create_node(data);
    if (newNode == NULL) {
        return -1;
    }
    
    /* If list is empty, insert at beginning */
    if (list->head == NULL) {
        list->head = newNode;
        list->count++;
        return 0;
    }
    
    /* Traverse to the last node */
    Node *current = list->head;
    while (current->next != NULL) {
        current = current->next;
    }
    
    current->next = newNode;
    list->count++;
    
    return 0;
}

/**
 * ll_insert_at_position()
 * Inserts node at specific position (1-indexed)
 * Position 1 = beginning, Position n = end, etc.
 */
int ll_insert_at_position(LinkedList *list, int position, int data) {
    if (list == NULL) {
        fprintf(stderr, "ERROR: List is NULL\n");
        return -1;
    }
    
    if (position < 1 || position > list->count + 1) {
        fprintf(stderr, "ERROR: Invalid position %d (list has %d nodes)\n", position, list->count);
        return -1;
    }
    
    /* If position is 1, insert at beginning */
    if (position == 1) {
        return ll_insert_beginning(list, data);
    }
    
    Node *newNode = ll_create_node(data);
    if (newNode == NULL) {
        return -1;
    }
    
    /* Traverse to position-1 node */
    Node *current = list->head;
    int count = 1;
    while (count < position - 1) {
        current = current->next;
        count++;
    }
    
    newNode->next = current->next;
    current->next = newNode;
    list->count++;
    
    return 0;
}

/* ========== DELETION OPERATIONS ========== */

/**
 * ll_delete_beginning()
 * Deletes the first node from the list
 * Returns the deleted node's data
 */
int ll_delete_beginning(LinkedList *list) {
    if (list == NULL || list->head == NULL) {
        fprintf(stderr, "ERROR: List is NULL or empty\n");
        return -1;
    }
    
    Node *temp = list->head;
    int data = temp->data;
    list->head = list->head->next;
    free(temp);
    list->count--;
    
    return data;
}

/**
 * ll_delete_end()
 * Deletes the last node from the list
 * Returns the deleted node's data
 */
int ll_delete_end(LinkedList *list) {
    if (list == NULL || list->head == NULL) {
        fprintf(stderr, "ERROR: List is NULL or empty\n");
        return -1;
    }
    
    /* If only one node, delete beginning */
    if (list->head->next == NULL) {
        return ll_delete_beginning(list);
    }
    
    /* Traverse to second-last node */
    Node *current = list->head;
    while (current->next->next != NULL) {
        current = current->next;
    }
    
    Node *lastNode = current->next;
    int data = lastNode->data;
    current->next = NULL;
    free(lastNode);
    list->count--;
    
    return data;
}

/**
 * ll_delete_at_position()
 * Deletes node at specific position (1-indexed)
 * Returns the deleted node's data
 */
int ll_delete_at_position(LinkedList *list, int position) {
    if (list == NULL || list->head == NULL) {
        fprintf(stderr, "ERROR: List is NULL or empty\n");
        return -1;
    }
    
    if (position < 1 || position > list->count) {
        fprintf(stderr, "ERROR: Invalid position %d (list has %d nodes)\n", position, list->count);
        return -1;
    }
    
    /* If position is 1, delete beginning */
    if (position == 1) {
        return ll_delete_beginning(list);
    }
    
    /* Traverse to position-1 node */
    Node *current = list->head;
    int count = 1;
    while (count < position - 1) {
        current = current->next;
        count++;
    }
    
    Node *temp = current->next;
    int data = temp->data;
    current->next = temp->next;
    free(temp);
    list->count--;
    
    return data;
}

/* ========== TRAVERSAL & DISPLAY ========== */

/**
 * ll_display()
 * Prints all elements in the list in forward order
 * Format: 10 -> 20 -> 30 -> NULL
 */
void ll_display(LinkedList *list) {
    if (list == NULL) {
        fprintf(stderr, "ERROR: List is NULL\n");
        return;
    }
    
    if (list->head == NULL) {
        printf("List is empty\n");
        return;
    }
    
    Node *current = list->head;
    printf("List: ");
    while (current != NULL) {
        printf("%d", current->data);
        if (current->next != NULL) {
            printf(" -> ");
        }
        current = current->next;
    }
    printf(" -> NULL (Total nodes: %d)\n", list->count);
}

/**
 * ll_display_reverse()
 * Displays list in reverse order using recursion
 * Format: 30 -> 20 -> 10 -> NULL
 */
void ll_display_reverse(Node *node) {
    if (node == NULL) {
        return;
    }
    
    ll_display_reverse(node->next);
    printf("%d", node->data);
    if (node->next != NULL) {
        printf(" <- ");
    }
}

/* ========== SEARCH & COUNT ========== */

/**
 * ll_search()
 * Searches for an element in the list
 * Returns position (1-indexed) if found, -1 if not found
 */
int ll_search(LinkedList *list, int data) {
    if (list == NULL || list->head == NULL) {
        fprintf(stderr, "ERROR: List is NULL or empty\n");
        return -1;
    }
    
    Node *current = list->head;
    int position = 1;
    
    while (current != NULL) {
        if (current->data == data) {
            return position;
        }
        current = current->next;
        position++;
    }
    
    return -1;  /* Element not found */
}

/**
 * ll_count()
 * Returns the total number of nodes in the list
 * Returns the count maintained in the LinkedList structure
 */
int ll_count(LinkedList *list) {
    if (list == NULL) {
        fprintf(stderr, "ERROR: List is NULL\n");
        return -1;
    }
    
    return list->count;
}

/* ========== LIST MANIPULATION ========== */

/**
 * ll_reverse()
 * Reverses the entire linked list in-place
 * Uses three pointers: prev, current, next
 */
int ll_reverse(LinkedList *list) {
    if (list == NULL) {
        fprintf(stderr, "ERROR: List is NULL\n");
        return -1;
    }
    
    if (list->head == NULL || list->head->next == NULL) {
        return 0;  /* Empty list or single node, already reversed */
    }
    
    Node *prev = NULL;
    Node *current = list->head;
    Node *next = NULL;
    
    while (current != NULL) {
        next = current->next;  /* Store next node */
        current->next = prev;  /* Reverse the link */
        prev = current;        /* Move prev one step */
        current = next;        /* Move current one step */
    }
    
    list->head = prev;  /* Update head to new beginning */
    return 0;
}

/**
 * ll_sort()
 * Sorts the list in ascending order using bubble sort algorithm
 * Compares adjacent nodes and swaps data if needed
 */
int ll_sort(LinkedList *list) {
    if (list == NULL || list->head == NULL) {
        fprintf(stderr, "ERROR: List is NULL or empty\n");
        return -1;
    }
    
    if (list->head->next == NULL) {
        return 0;  /* Single node, already sorted */
    }
    
    Node *i, *j;
    int swapped;
    
    for (i = list->head; i != NULL; i = i->next) {
        swapped = 0;
        j = list->head;
        
        while (j->next != NULL) {
            if (j->data > j->next->data) {
                /* Swap data */
                int temp = j->data;
                j->data = j->next->data;
                j->next->data = temp;
                swapped = 1;
            }
            j = j->next;
        }
        
        if (!swapped) {
            break;  /* List is sorted */
        }
    }
    
    return 0;
}

/**
 * ll_detect_loop()
 * Detects if there is a loop in the list using Floyd's Cycle Detection
 * (Tortoise and Hare algorithm)
 * Returns: 1 if loop exists, 0 if no loop, -1 on error
 */
int ll_detect_loop(LinkedList *list) {
    if (list == NULL || list->head == NULL) {
        fprintf(stderr, "ERROR: List is NULL or empty\n");
        return -1;
    }
    
    Node *slow = list->head;    /* Moves one step at a time */
    Node *fast = list->head;    /* Moves two steps at a time */
    
    while (fast != NULL && fast->next != NULL) {
        slow = slow->next;           /* Move one step */
        fast = fast->next->next;     /* Move two steps */
        
        if (slow == fast) {
            return 1;  /* Loop detected */
        }
    }
    
    return 0;  /* No loop detected */
}

/* ========== MEMORY MANAGEMENT ========== */

/**
 * ll_clear()
 * Deletes all nodes and clears the list
 * Sets head to NULL and count to 0
 */
void ll_clear(LinkedList *list) {
    if (list == NULL) {
        fprintf(stderr, "ERROR: List is NULL\n");
        return;
    }
    
    Node *current = list->head;
    Node *temp;
    
    while (current != NULL) {
        temp = current;
        current = current->next;
        free(temp);
    }
    
    list->head = NULL;
    list->count = 0;
}

/**
 * ll_free_list()
 * Completely frees the linked list and the LinkedList structure
 * Sets list pointer to NULL after freeing
 */
void ll_free_list(LinkedList **list) {
    if (list == NULL || *list == NULL) {
        fprintf(stderr, "ERROR: List pointer is NULL\n");
        return;
    }
    
    ll_clear(*list);
    free(*list);
    *list = NULL;
}

/* ========== UTILITY FUNCTIONS ========== */

/**
 * ll_is_empty()
 * Checks if the list is empty
 * Returns 1 if empty, 0 if not empty
 */
int ll_is_empty(LinkedList *list) {
    if (list == NULL) {
        fprintf(stderr, "ERROR: List is NULL\n");
        return -1;
    }
    
    return (list->head == NULL) ? 1 : 0;
}

/**
 * ll_get_middle()
 * Finds and returns the middle element of the list
 * Uses slow and fast pointers (tortoise and hare)
 * Returns the middle element value, -1 if list is empty
 */
int ll_get_middle(LinkedList *list) {
    if (list == NULL || list->head == NULL) {
        fprintf(stderr, "ERROR: List is NULL or empty\n");
        return -1;
    }
    
    Node *slow = list->head;
    Node *fast = list->head;
    
    while (fast != NULL && fast->next != NULL) {
        slow = slow->next;
        fast = fast->next->next;
    }
    
    return slow->data;
}
