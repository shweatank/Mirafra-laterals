/**
 * test_linkedlist.c
 * 
 * Comprehensive test suite for LinkedList implementation.
 * Tests all 20 operations and edge cases.
 * 
 * Compile: gcc -Wall -Wextra -g -o test_linkedlist test_linkedlist.c ../src/linkedlist.c -I../src
 * Run: ./test_linkedlist
 */

#include <stdio.h>
#include <stdlib.h>
#include "../src/linkedlist.h"
#include "test_runner.h"

/* Global test counters */
int tests_passed = 0;
int tests_failed = 0;

/*
 * ============================================================================
 *  INSERT TESTS
 * ============================================================================
 */

void test_insertAtBeginning(void) {
    LinkedList* list = createList();
    
    insertAtBeginning(list, 10);
    ASSERT_EQ(list->head->data, 10, "Insert at beginning: head value");
    ASSERT_EQ(list->size, 1, "Insert at beginning: size incremented");
    
    insertAtBeginning(list, 20);
    ASSERT_EQ(list->head->data, 20, "Insert at beginning: new head");
    ASSERT_EQ(list->head->next->data, 10, "Insert at beginning: old head link");
    ASSERT_EQ(list->size, 2, "Insert at beginning: size incremented again");
    
    destroyList(list);
}

void test_insertAtEnd(void) {
    LinkedList* list = createList();
    
    insertAtEnd(list, 10);
    ASSERT_EQ(list->head->data, 10, "Insert at end: single node");
    ASSERT_EQ(list->size, 1, "Insert at end: size is 1");
    
    insertAtEnd(list, 20);
    ASSERT_EQ(list->head->data, 10, "Insert at end: head unchanged");
    ASSERT_EQ(list->head->next->data, 20, "Insert at end: tail is 20");
    ASSERT_EQ(list->size, 2, "Insert at end: size is 2");
    
    insertAtEnd(list, 30);
    ASSERT_EQ(list->head->next->next->data, 30, "Insert at end: third element");
    ASSERT_EQ(list->size, 3, "Insert at end: size is 3");
    
    destroyList(list);
}

void test_insertAtPosition(void) {
    LinkedList* list = createList();
    
    insertAtPosition(list, 10, 0);
    insertAtPosition(list, 20, 1);
    insertAtPosition(list, 30, 2);
    
    insertAtPosition(list, 15, 1);
    ASSERT_EQ(list->head->data, 10, "Insert at position: first element");
    ASSERT_EQ(list->head->next->data, 15, "Insert at position: inserted element");
    ASSERT_EQ(list->head->next->next->data, 20, "Insert at position: shifted element");
    ASSERT_EQ(list->size, 4, "Insert at position: size updated");
    
    int result = insertAtPosition(list, 99, 10);
    ASSERT_EQ(result, 0, "Insert at position: invalid position rejected");
    
    destroyList(list);
}

/*
 * ============================================================================
 *  DELETE TESTS
 * ============================================================================
 */

void test_deleteAtBeginning(void) {
    LinkedList* list = createList();
    
    insertAtEnd(list, 10);
    insertAtEnd(list, 20);
    insertAtEnd(list, 30);
    
    int result = deleteAtBeginning(list);
    ASSERT_EQ(result, 1, "Delete at beginning: success");
    ASSERT_EQ(list->head->data, 20, "Delete at beginning: new head");
    ASSERT_EQ(list->size, 2, "Delete at beginning: size decremented");
    
    destroyList(list);
}

void test_deleteAtEnd(void) {
    LinkedList* list = createList();
    
    insertAtEnd(list, 10);
    insertAtEnd(list, 20);
    insertAtEnd(list, 30);
    
    int result = deleteAtEnd(list);
    ASSERT_EQ(result, 1, "Delete at end: success");
    ASSERT_EQ(list->head->next->data, 20, "Delete at end: last node removed");
    ASSERT_EQ(list->head->next->next, NULL, "Delete at end: NULL pointer set");
    ASSERT_EQ(list->size, 2, "Delete at end: size decremented");
    
    destroyList(list);
}

void test_deleteAtPosition(void) {
    LinkedList* list = createList();
    
    insertAtEnd(list, 10);
    insertAtEnd(list, 20);
    insertAtEnd(list, 30);
    insertAtEnd(list, 40);
    
    int result = deleteAtPosition(list, 1);
    ASSERT_EQ(result, 1, "Delete at position: success");
    ASSERT_EQ(list->head->data, 10, "Delete at position: first unchanged");
    ASSERT_EQ(list->head->next->data, 30, "Delete at position: element removed");
    ASSERT_EQ(list->size, 3, "Delete at position: size decremented");
    
    destroyList(list);
}

void test_deleteByValue(void) {
    LinkedList* list = createList();
    
    insertAtEnd(list, 10);
    insertAtEnd(list, 20);
    insertAtEnd(list, 30);
    
    int result = deleteByValue(list, 20);
    ASSERT_EQ(result, 1, "Delete by value: found and deleted");
    ASSERT_EQ(list->head->data, 10, "Delete by value: first unchanged");
    ASSERT_EQ(list->head->next->data, 30, "Delete by value: next correct");
    ASSERT_EQ(list->size, 2, "Delete by value: size decremented");
    
    result = deleteByValue(list, 99);
    ASSERT_EQ(result, 0, "Delete by value: not found");
    
    destroyList(list);
}

void test_deleteByNode(void) {
    LinkedList* list = createList();
    
    insertAtEnd(list, 10);
    insertAtEnd(list, 20);
    insertAtEnd(list, 30);
    insertAtEnd(list, 40);
    
    /* Get reference to second node (20) */
    Node* node_to_delete = list->head->next;
    ASSERT_EQ(node_to_delete->data, 20, "Delete by node: correct node reference");
    
    int result = deleteByNode(list, node_to_delete);
    ASSERT_EQ(result, 1, "Delete by node: node deleted successfully");
    ASSERT_EQ(list->head->data, 10, "Delete by node: head unchanged");
    ASSERT_EQ(list->head->next->data, 30, "Delete by node: next node updated");
    ASSERT_EQ(list->size, 3, "Delete by node: size decremented");
    
    /* Try to delete head node */
    Node* head_node = list->head;
    result = deleteByNode(list, head_node);
    ASSERT_EQ(result, 1, "Delete by node: head node deleted");
    ASSERT_EQ(list->head->data, 30, "Delete by node: new head set");
    ASSERT_EQ(list->size, 2, "Delete by node: size decremented again");
    
    /* Try to delete NULL node */
    result = deleteByNode(list, NULL);
    ASSERT_EQ(result, 0, "Delete by node: NULL node returns 0");
    
    destroyList(list);
}

/*
 * ============================================================================
 *  SEARCH TESTS
 * ============================================================================
 */

void test_search(void) {
    LinkedList* list = createList();
    
    insertAtEnd(list, 10);
    insertAtEnd(list, 20);
    insertAtEnd(list, 30);
    
    ASSERT_EQ(search(list, 10), 0, "Search: first element");
    ASSERT_EQ(search(list, 20), 1, "Search: middle element");
    ASSERT_EQ(search(list, 30), 2, "Search: last element");
    ASSERT_EQ(search(list, 99), -1, "Search: not found");
    
    destroyList(list);
}

void test_getLength(void) {
    LinkedList* list = createList();
    
    ASSERT_EQ(getLength(list), 0, "Get length: empty list");
    
    insertAtEnd(list, 10);
    ASSERT_EQ(getLength(list), 1, "Get length: 1 element");
    
    insertAtEnd(list, 20);
    insertAtEnd(list, 30);
    ASSERT_EQ(getLength(list), 3, "Get length: 3 elements");
    
    destroyList(list);
}

/*
 * ============================================================================
 *  MIDDLE NODE TESTS
 * ============================================================================
 */

void test_findMiddle_odd(void) {
    LinkedList* list = createList();
    
    insertAtEnd(list, 10);
    insertAtEnd(list, 20);
    insertAtEnd(list, 30);
    
    Node* middle = findMiddle(list);
    ASSERT_EQ(middle->data, 20, "Find middle: odd length list");
    
    destroyList(list);
}

void test_findMiddle_even(void) {
    LinkedList* list = createList();
    
    insertAtEnd(list, 10);
    insertAtEnd(list, 20);
    insertAtEnd(list, 30);
    insertAtEnd(list, 40);
    
    Node* middle = findMiddle(list);
    ASSERT_EQ(middle->data, 30, "Find middle: even length list");
    
    destroyList(list);
}

/*
 * ============================================================================
 *  NTH FROM END TESTS
 * ============================================================================
 */

void test_nthFromEnd(void) {
    LinkedList* list = createList();
    
    insertAtEnd(list, 10);
    insertAtEnd(list, 20);
    insertAtEnd(list, 30);
    insertAtEnd(list, 40);
    insertAtEnd(list, 50);
    
    Node* nth = nthFromEnd(list, 1);
    ASSERT_EQ(nth->data, 50, "Nth from end: 1st from end");
    
    nth = nthFromEnd(list, 2);
    ASSERT_EQ(nth->data, 40, "Nth from end: 2nd from end");
    
    nth = nthFromEnd(list, 5);
    ASSERT_EQ(nth->data, 10, "Nth from end: 5th from end");
    
    nth = nthFromEnd(list, 10);
    ASSERT_NULL(nth, "Nth from end: invalid n");
    
    destroyList(list);
}

/*
 * ============================================================================
 *  REVERSE TESTS
 * ============================================================================
 */

void test_reverseList(void) {
    LinkedList* list = createList();
    
    insertAtEnd(list, 10);
    insertAtEnd(list, 20);
    insertAtEnd(list, 30);
    
    reverseList(list);
    
    ASSERT_EQ(list->head->data, 30, "Reverse: first element");
    ASSERT_EQ(list->head->next->data, 20, "Reverse: second element");
    ASSERT_EQ(list->head->next->next->data, 10, "Reverse: third element");
    
    destroyList(list);
}

/*
 * ============================================================================
 *  SORT TESTS
 * ============================================================================
 */

void test_sortList_bubble(void) {
    LinkedList* list = createList();
    
    insertAtEnd(list, 50);
    insertAtEnd(list, 20);
    insertAtEnd(list, 40);
    insertAtEnd(list, 10);
    insertAtEnd(list, 30);
    
    sortList(list);
    
    ASSERT_EQ(list->head->data, 10, "Sort bubble: first element");
    ASSERT_EQ(list->head->next->data, 20, "Sort bubble: second element");
    ASSERT_EQ(list->head->next->next->data, 30, "Sort bubble: third element");
    ASSERT_EQ(list->head->next->next->next->data, 40, "Sort bubble: fourth element");
    ASSERT_EQ(list->head->next->next->next->next->data, 50, "Sort bubble: fifth element");
    
    destroyList(list);
}

void test_mergeSort_list(void) {
    LinkedList* list = createList();
    
    insertAtEnd(list, 50);
    insertAtEnd(list, 20);
    insertAtEnd(list, 40);
    insertAtEnd(list, 10);
    insertAtEnd(list, 30);
    
    mergeSort(list);
    
    ASSERT_EQ(list->head->data, 10, "Merge sort: first element");
    ASSERT_EQ(list->head->next->data, 20, "Merge sort: second element");
    ASSERT_EQ(list->head->next->next->data, 30, "Merge sort: third element");
    
    destroyList(list);
}

/*
 * ============================================================================
 *  MERGE TWO LISTS TESTS
 * ============================================================================
 */

void test_mergeTwoLists(void) {
    LinkedList* list1 = createList();
    LinkedList* list2 = createList();
    
    insertAtEnd(list1, 10);
    insertAtEnd(list1, 30);
    insertAtEnd(list1, 50);
    
    insertAtEnd(list2, 20);
    insertAtEnd(list2, 40);
    insertAtEnd(list2, 60);
    
    LinkedList* merged = mergeTwoLists(list1, list2);
    
    ASSERT_EQ(merged->head->data, 10, "Merge two lists: first element");
    ASSERT_EQ(merged->head->next->data, 20, "Merge two lists: second element");
    ASSERT_EQ(merged->head->next->next->data, 30, "Merge two lists: third element");
    ASSERT_EQ(merged->size, 6, "Merge two lists: correct size");
    
    destroyList(list1);
    destroyList(list2);
    destroyList(merged);
}

/*
 * ============================================================================
 *  REMOVE DUPLICATES TESTS
 * ============================================================================
 */

void test_removeDuplicates(void) {
    LinkedList* list = createList();
    
    insertAtEnd(list, 10);
    insertAtEnd(list, 20);
    insertAtEnd(list, 20);
    insertAtEnd(list, 30);
    insertAtEnd(list, 30);
    insertAtEnd(list, 30);
    
    removeDuplicates(list);
    
    ASSERT_EQ(list->head->data, 10, "Remove duplicates: first element");
    ASSERT_EQ(list->head->next->data, 20, "Remove duplicates: second element");
    ASSERT_EQ(list->head->next->next->data, 30, "Remove duplicates: third element");
    ASSERT_EQ(list->size, 3, "Remove duplicates: correct size");
    
    destroyList(list);
}

/*
 * ============================================================================
 *  LOOP DETECTION TESTS
 * ============================================================================
 */

void test_detectLoop(void) {
    LinkedList* list = createList();
    
    insertAtEnd(list, 10);
    insertAtEnd(list, 20);
    insertAtEnd(list, 30);
    
    ASSERT_EQ(detectLoop(list), 0, "Detect loop: no loop in normal list");
    
    /* Create artificial loop: last node points to head */
    Node* current = list->head;
    while (current->next != NULL) {
        current = current->next;
    }
    current->next = list->head;
    
    ASSERT_EQ(detectLoop(list), 1, "Detect loop: loop detected");
    
    /* Clean up: break loop before destroying */
    current->next = NULL;
    destroyList(list);
}

/*
 * ============================================================================
 *  EDGE CASE TESTS
 * ============================================================================
 */

void test_edge_case_empty_list(void) {
    LinkedList* list = createList();
    
    ASSERT_EQ(deleteAtBeginning(list), 0, "Delete from empty: returns 0");
    ASSERT_EQ(deleteAtEnd(list), 0, "Delete from empty: returns 0");
    ASSERT_EQ(search(list, 10), -1, "Search in empty: returns -1");
    ASSERT_NULL(findMiddle(list), "Find middle in empty: NULL");
    
    destroyList(list);
}

void test_edge_case_single_node(void) {
    LinkedList* list = createList();
    
    insertAtEnd(list, 10);
    
    Node* middle = findMiddle(list);
    ASSERT_EQ(middle->data, 10, "Find middle: single node");
    
    Node* nth = nthFromEnd(list, 1);
    ASSERT_EQ(nth->data, 10, "Nth from end: single node");
    
    destroyList(list);
}

void test_edge_case_memory_allocation(void) {
    LinkedList* list = createList();
    ASSERT_NOT_NULL(list, "Create list: memory allocated");
    
    int result = insertAtEnd(list, 10);
    ASSERT_EQ(result, 1, "Insert: memory allocated successfully");
    
    destroyList(list);
}

/*
 * ============================================================================
 *  MAIN TEST RUNNER
 * ============================================================================
 */

int main(void) {
    TEST_HEADER("LinkedList Comprehensive Test Suite");
    
    /* Insert Tests */
    test_insertAtBeginning();
    test_insertAtEnd();
    test_insertAtPosition();
    
    /* Delete Tests */
    test_deleteAtBeginning();
    test_deleteAtEnd();
    test_deleteAtPosition();
    test_deleteByValue();
    test_deleteByNode();
    
    /* Search Tests */
    test_search();
    test_getLength();
    
    /* Middle Node Tests */
    test_findMiddle_odd();
    test_findMiddle_even();
    
    /* Nth From End Tests */
    test_nthFromEnd();
    
    /* Reverse Tests */
    test_reverseList();
    
    /* Sort Tests */
    test_sortList_bubble();
    test_mergeSort_list();
    
    /* Merge Two Lists Tests */
    test_mergeTwoLists();
    
    /* Remove Duplicates Tests */
    test_removeDuplicates();
    
    /* Loop Detection Tests */
    test_detectLoop();
    
    /* Edge Case Tests */
    test_edge_case_empty_list();
    test_edge_case_single_node();
    test_edge_case_memory_allocation();
    
    TEST_FOOTER();
    
    return (tests_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
