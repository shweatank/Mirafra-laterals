/**
 * linkedlist.c
 * 
 * Complete implementation of LinkedList operations in C.
 * All 20 operations are fully implemented with detailed comments.
 * 
 * Compiler: gcc -Wall -Wextra -g
 * Standard: C99
 */

#include "linkedlist.h"

/*
 * ============================================================================
 *  CORE OPERATIONS IMPLEMENTATION
 * ============================================================================
 */

/**
 * createList - Initialize an empty linked list.
 */
LinkedList* createList(void) {
    LinkedList* list = (LinkedList*)malloc(sizeof(LinkedList));
    if (list == NULL) {
        fprintf(stderr, "Error: Memory allocation failed for LinkedList\n");
        return NULL;
    }
    list->head = NULL;
    list->size = 0;
    return list;
}

/**
 * destroyList - Free all memory allocated for the linked list.
 */
void destroyList(LinkedList* list) {
    if (list == NULL) {
        return;
    }
    
    Node* current = list->head;
    while (current != NULL) {
        Node* temp = current;
        current = current->next;
        free(temp);
    }
    
    free(list);
}

/*
 * ============================================================================
 *  INSERT OPERATIONS IMPLEMENTATION
 * ============================================================================
 */

/**
 * insertAtBeginning - Insert a node at the beginning of the list.
 */
int insertAtBeginning(LinkedList* list, int data) {
    if (list == NULL) {
        return 0;
    }
    
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL) {
        fprintf(stderr, "Error: Memory allocation failed for new node\n");
        return 0;
    }
    
    newNode->data = data;
    newNode->next = list->head;
    list->head = newNode;
    list->size++;
    
    return 1;
}

/**
 * insertAtEnd - Insert a node at the end of the list.
 */
int insertAtEnd(LinkedList* list, int data) {
    if (list == NULL) {
        return 0;
    }
    
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL) {
        fprintf(stderr, "Error: Memory allocation failed for new node\n");
        return 0;
    }
    
    newNode->data = data;
    newNode->next = NULL;
    
    if (list->head == NULL) {
        list->head = newNode;
        list->size++;
        return 1;
    }
    
    Node* current = list->head;
    while (current->next != NULL) {
        current = current->next;
    }
    
    current->next = newNode;
    list->size++;
    
    return 1;
}

/**
 * insertAtPosition - Insert a node at a specific position (0-indexed).
 */
int insertAtPosition(LinkedList* list, int data, int position) {
    if (list == NULL || position < 0 || position > list->size) {
        return 0;
    }
    
    if (position == 0) {
        return insertAtBeginning(list, data);
    }
    
    if (position == list->size) {
        return insertAtEnd(list, data);
    }
    
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL) {
        fprintf(stderr, "Error: Memory allocation failed for new node\n");
        return 0;
    }
    
    newNode->data = data;
    
    Node* current = list->head;
    for (int i = 0; i < position - 1; i++) {
        current = current->next;
    }
    
    newNode->next = current->next;
    current->next = newNode;
    list->size++;
    
    return 1;
}

/*
 * ============================================================================
 *  DELETE OPERATIONS IMPLEMENTATION
 * ============================================================================
 */

/**
 * deleteAtBeginning - Remove the first node from the list.
 */
int deleteAtBeginning(LinkedList* list) {
    if (list == NULL || list->head == NULL) {
        return 0;
    }
    
    Node* temp = list->head;
    list->head = list->head->next;
    free(temp);
    list->size--;
    
    return 1;
}

/**
 * deleteAtEnd - Remove the last node from the list.
 */
int deleteAtEnd(LinkedList* list) {
    if (list == NULL || list->head == NULL) {
        return 0;
    }
    
    if (list->head->next == NULL) {
        free(list->head);
        list->head = NULL;
        list->size--;
        return 1;
    }
    
    Node* current = list->head;
    while (current->next->next != NULL) {
        current = current->next;
    }
    
    free(current->next);
    current->next = NULL;
    list->size--;
    
    return 1;
}

/**
 * deleteAtPosition - Remove the node at a specific position.
 */
int deleteAtPosition(LinkedList* list, int position) {
    if (list == NULL || position < 0 || position >= list->size) {
        return 0;
    }
    
    if (position == 0) {
        return deleteAtBeginning(list);
    }
    
    Node* current = list->head;
    for (int i = 0; i < position - 1; i++) {
        current = current->next;
    }
    
    Node* temp = current->next;
    current->next = temp->next;
    free(temp);
    list->size--;
    
    return 1;
}

/**
 * deleteByValue - Find and remove the first node with the given value.
 */
int deleteByValue(LinkedList* list, int value) {
    if (list == NULL || list->head == NULL) {
        return 0;
    }
    
    if (list->head->data == value) {
        return deleteAtBeginning(list);
    }
    
    Node* current = list->head;
    while (current->next != NULL) {
        if (current->next->data == value) {
            Node* temp = current->next;
            current->next = temp->next;
            free(temp);
            list->size--;
            return 1;
        }
        current = current->next;
    }
    
    return 0;
}

/**
 * deleteByNode - Remove a specific node from the list.
 */
int deleteByNode(LinkedList* list, Node* node) {
    if (list == NULL || node == NULL || list->head == NULL) {
        return 0;
    }
    
    /* If the node to delete is the head */
    if (list->head == node) {
        return deleteAtBeginning(list);
    }
    
    /* Search for the node and its predecessor */
    Node* current = list->head;
    while (current != NULL && current->next != NULL) {
        if (current->next == node) {
            /* Found the node, remove it */
            Node* temp = current->next;
            current->next = temp->next;
            free(temp);
            list->size--;
            return 1;
        }
        current = current->next;
    }
    
    /* Node not found in list */
    return 0;
}

/*
 * ============================================================================
 *  SEARCH AND ACCESS OPERATIONS IMPLEMENTATION
 * ============================================================================
 */

/**
 * search - Find the index of a node with the given value.
 */
int search(LinkedList* list, int value) {
    if (list == NULL || list->head == NULL) {
        return -1;
    }
    
    Node* current = list->head;
    int index = 0;
    
    while (current != NULL) {
        if (current->data == value) {
            return index;
        }
        current = current->next;
        index++;
    }
    
    return -1;
}

/**
 * getLength - Return the number of nodes in the list.
 */
int getLength(LinkedList* list) {
    if (list == NULL) {
        return 0;
    }
    return list->size;
}

/**
 * findMiddle - Return a pointer to the middle node using slow/fast pointers.
 */
Node* findMiddle(LinkedList* list) {
    if (list == NULL || list->head == NULL) {
        return NULL;
    }
    
    Node* slow = list->head;
    Node* fast = list->head;
    
    while (fast != NULL && fast->next != NULL) {
        slow = slow->next;
        fast = fast->next->next;
    }
    
    return slow;
}

/**
 * nthFromEnd - Get the nth node from the end (1-indexed).
 */
Node* nthFromEnd(LinkedList* list, int n) {
    if (list == NULL || list->head == NULL || n <= 0 || n > list->size) {
        return NULL;
    }
    
    Node* first = list->head;
    Node* second = list->head;
    
    for (int i = 0; i < n; i++) {
        if (first == NULL) {
            return NULL;
        }
        first = first->next;
    }
    
    while (first != NULL) {
        first = first->next;
        second = second->next;
    }
    
    return second;
}

/*
 * ============================================================================
 *  DISPLAY AND UTILITY OPERATIONS IMPLEMENTATION
 * ============================================================================
 */

/**
 * printList - Print all node values in the list.
 */
void printList(LinkedList* list) {
    if (list == NULL) {
        printf("NULL\n");
        return;
    }
    
    Node* current = list->head;
    printf("[");
    
    while (current != NULL) {
        printf("%d", current->data);
        if (current->next != NULL) {
            printf("] -> [");
        }
        current = current->next;
    }
    
    printf("] -> NULL\n");
}

/*
 * ============================================================================
 *  SORTING AND REORDERING OPERATIONS IMPLEMENTATION
 * ============================================================================
 */

/**
 * reverseList - Reverse the order of all nodes in-place.
 */
void reverseList(LinkedList* list) {
    if (list == NULL || list->head == NULL) {
        return;
    }
    
    Node* prev = NULL;
    Node* current = list->head;
    Node* next = NULL;
    
    while (current != NULL) {
        next = current->next;
        current->next = prev;
        prev = current;
        current = next;
    }
    
    list->head = prev;
}

/**
 * sortList - Sort the list in ascending order using bubble sort.
 */
void sortList(LinkedList* list) {
    if (list == NULL || list->head == NULL || list->head->next == NULL) {
        return;
    }
    
    for (int i = 0; i < list->size - 1; i++) {
        Node* current = list->head;
        
        while (current->next != NULL) {
            if (current->data > current->next->data) {
                int temp = current->data;
                current->data = current->next->data;
                current->next->data = temp;
            }
            current = current->next;
        }
    }
}

/**
 * Helper function for merge sort: merge two sorted lists.
 */
static Node* merge(Node* left, Node* right) {
    if (left == NULL) {
        return right;
    }
    if (right == NULL) {
        return left;
    }
    
    if (left->data <= right->data) {
        left->next = merge(left->next, right);
        return left;
    } else {
        right->next = merge(left, right->next);
        return right;
    }
}

/**
 * Helper function for merge sort: split list into two halves.
 */
static void split(Node* head, Node** left, Node** right) {
    Node* slow = head;
    Node* fast = head->next;
    
    while (fast != NULL && fast->next != NULL) {
        slow = slow->next;
        fast = fast->next->next;
    }
    
    *left = head;
    *right = slow->next;
    slow->next = NULL;
}

/**
 * Helper function for merge sort: recursive merge sort.
 */
static Node* mergeSortHelper(Node* head) {
    if (head == NULL || head->next == NULL) {
        return head;
    }
    
    Node* left = NULL;
    Node* right = NULL;
    
    split(head, &left, &right);
    
    left = mergeSortHelper(left);
    right = mergeSortHelper(right);
    
    return merge(left, right);
}

/**
 * mergeSort - Sort the list in ascending order using merge sort.
 */
void mergeSort(LinkedList* list) {
    if (list == NULL || list->head == NULL) {
        return;
    }
    
    list->head = mergeSortHelper(list->head);
}

/**
 * mergeTwoLists - Merge two sorted linked lists into one sorted list.
 */
LinkedList* mergeTwoLists(LinkedList* list1, LinkedList* list2) {
    if (list1 == NULL || list2 == NULL) {
        return NULL;
    }
    
    LinkedList* result = createList();
    if (result == NULL) {
        return NULL;
    }
    
    Node* ptr1 = list1->head;
    Node* ptr2 = list2->head;
    
    while (ptr1 != NULL && ptr2 != NULL) {
        if (ptr1->data <= ptr2->data) {
            if (!insertAtEnd(result, ptr1->data)) {
                destroyList(result);
                return NULL;
            }
            ptr1 = ptr1->next;
        } else {
            if (!insertAtEnd(result, ptr2->data)) {
                destroyList(result);
                return NULL;
            }
            ptr2 = ptr2->next;
        }
    }
    
    while (ptr1 != NULL) {
        if (!insertAtEnd(result, ptr1->data)) {
            destroyList(result);
            return NULL;
        }
        ptr1 = ptr1->next;
    }
    
    while (ptr2 != NULL) {
        if (!insertAtEnd(result, ptr2->data)) {
            destroyList(result);
            return NULL;
        }
        ptr2 = ptr2->next;
    }
    
    return result;
}

/*
 * ============================================================================
 *  ADVANCED OPERATIONS IMPLEMENTATION
 * ============================================================================
 */

/**
 * removeDuplicates - Remove all duplicate values from the list.
 */
void removeDuplicates(LinkedList* list) {
    if (list == NULL || list->head == NULL) {
        return;
    }
    
    Node* current = list->head;
    
    while (current != NULL && current->next != NULL) {
        if (current->data == current->next->data) {
            Node* temp = current->next;
            current->next = temp->next;
            free(temp);
            list->size--;
        } else {
            current = current->next;
        }
    }
}

/**
 * detectLoop - Detect if there is a cycle using Floyd's algorithm.
 */
int detectLoop(LinkedList* list) {
    if (list == NULL || list->head == NULL) {
        return 0;
    }
    
    Node* slow = list->head;
    Node* fast = list->head;
    
    while (fast != NULL && fast->next != NULL) {
        slow = slow->next;
        fast = fast->next->next;
        
        if (slow == fast) {
            return 1;
        }
    }
    
    return 0;
}
