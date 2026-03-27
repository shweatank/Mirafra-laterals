/*
 * ============================================================================
 * FILE: test_linkedlist.c
 * DESC: Comprehensive test suite for all linked list operations
 * AUTH: System Programmer
 * DATE: March 27, 2026
 * ============================================================================
 */

#include <assert.h>
#include "../include/linkedlist.h"

/* Test counters */
int total_tests = 0;
int passed_tests = 0;
int failed_tests = 0;

/* Log file for test results */
FILE *log_file = NULL;

/* ========== TEST UTILITY FUNCTIONS ========== */

/**
 * test_init()
 * Opens log file for appending test results
 */
void test_init(void) {
    log_file = fopen("logs/testlog.txt", "a");
    if (log_file == NULL) {
        fprintf(stderr, "ERROR: Could not open logs/testlog.txt\n");
        exit(1);
    }
    
    /* Write timestamp */
    time_t now = time(NULL);
    struct tm *timeinfo = localtime(&now);
    char timestamp[100];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", timeinfo);
    
    fprintf(log_file, "\n");
    fprintf(log_file, "═════════════════════════════════════════════════\n");
    fprintf(log_file, "Test Run: %s\n", timestamp);
    fprintf(log_file, "═════════════════════════════════════════════════\n");
    printf("═════════════════════════════════════════════════\n");
    printf("Test Run: %s\n", timestamp);
    printf("═════════════════════════════════════════════════\n");
}

/**
 * test_cleanup()
 * Closes log file and prints summary
 */
void test_cleanup(void) {
    printf("\n─────────────────────────────────────────────────\n");
    printf("TEST SUMMARY\n");
    printf("─────────────────────────────────────────────────\n");
    printf("Total Tests:  %d\n", total_tests);
    printf("Passed:       %d ✓\n", passed_tests);
    printf("Failed:       %d ✗\n", failed_tests);
    printf("─────────────────────────────────────────────────\n\n");
    
    fprintf(log_file, "\n─────────────────────────────────────────────────\n");
    fprintf(log_file, "TEST SUMMARY\n");
    fprintf(log_file, "─────────────────────────────────────────────────\n");
    fprintf(log_file, "Total Tests:  %d\n", total_tests);
    fprintf(log_file, "Passed:       %d\n", passed_tests);
    fprintf(log_file, "Failed:       %d\n", failed_tests);
    fprintf(log_file, "─────────────────────────────────────────────────\n\n");
    
    fclose(log_file);
}

/**
 * assert_true()
 * Custom assertion function for test cases
 */
void assert_true(int condition, const char *test_name) {
    total_tests++;
    
    if (condition) {
        printf("✓ PASS: %s\n", test_name);
        fprintf(log_file, "✓ PASS: %s\n", test_name);
        passed_tests++;
    } else {
        printf("✗ FAIL: %s\n", test_name);
        fprintf(log_file, "✗ FAIL: %s\n", test_name);
        failed_tests++;
    }
}

/**
 * assert_equal()
 * Compares two integers and logs result
 */
void assert_equal(int expected, int actual, const char *test_name) {
    total_tests++;
    
    if (expected == actual) {
        printf("✓ PASS: %s (expected %d, got %d)\n", test_name, expected, actual);
        fprintf(log_file, "✓ PASS: %s (expected %d, got %d)\n", test_name, expected, actual);
        passed_tests++;
    } else {
        printf("✗ FAIL: %s (expected %d, got %d)\n", test_name, expected, actual);
        fprintf(log_file, "✗ FAIL: %s (expected %d, got %d)\n", test_name, expected, actual);
        failed_tests++;
    }
}

/* ========== TEST CASES ========== */

/**
 * test_create_list()
 * Tests: Creating empty linked list
 */
void test_create_list(void) {
    printf("\n─── Test Group: CREATE OPERATIONS ───\n");
    
    LinkedList *list = ll_create_list();
    assert_true(list != NULL, "Create list returns non-NULL pointer");
    assert_true(list->head == NULL, "New list head is NULL");
    assert_equal(0, list->count, "New list count is 0");
    assert_true(ll_is_empty(list), "New list is empty");
    
    ll_free_list(&list);
}

/**
 * test_insert_beginning()
 * Tests: Insert at beginning
 */
void test_insert_beginning(void) {
    printf("\n─── Test Group: INSERT AT BEGINNING ───\n");
    
    LinkedList *list = ll_create_list();
    
    /* Insert single element */
    int result = ll_insert_beginning(list, 10);
    assert_equal(0, result, "Insert at beginning returns 0");
    assert_equal(10, list->head->data, "First element is 10");
    assert_equal(1, list->count, "Count is 1 after insertion");
    
    /* Insert another at beginning */
    ll_insert_beginning(list, 5);
    assert_equal(5, list->head->data, "Head is now 5");
    assert_equal(10, list->head->next->data, "Second node is 10");
    assert_equal(2, list->count, "Count is 2");
    
    /* Insert third */
    ll_insert_beginning(list, 1);
    assert_equal(1, list->head->data, "Head is now 1");
    assert_equal(3, list->count, "Count is 3");
    
    ll_free_list(&list);
}

/**
 * test_insert_end()
 * Tests: Insert at end
 */
void test_insert_end(void) {
    printf("\n─── Test Group: INSERT AT END ───\n");
    
    LinkedList *list = ll_create_list();
    
    /* Insert to empty list */
    int result = ll_insert_end(list, 10);
    assert_equal(0, result, "Insert at end returns 0");
    assert_equal(10, list->head->data, "Single element inserted at head");
    assert_equal(1, list->count, "Count is 1");
    
    /* Insert more elements */
    ll_insert_end(list, 20);
    ll_insert_end(list, 30);
    assert_equal(3, list->count, "Count is 3");
    assert_equal(10, list->head->data, "Head is still 10");
    assert_equal(30, list->head->next->next->data, "Last element is 30");
    
    ll_free_list(&list);
}

/**
 * test_insert_at_position()
 * Tests: Insert at specific position
 */
void test_insert_at_position(void) {
    printf("\n─── Test Group: INSERT AT POSITION ───\n");
    
    LinkedList *list = ll_create_list();
    ll_insert_end(list, 10);
    ll_insert_end(list, 20);
    ll_insert_end(list, 40);
    
    /* Insert at position 2 */
    int result = ll_insert_at_position(list, 2, 15);
    assert_equal(0, result, "Insert at position 2 returns 0");
    assert_equal(4, list->count, "Count increased to 4");
    assert_equal(15, list->head->next->data, "Element at position 2 is 15");
    
    /* Insert at position 1 (beginning) */
    ll_insert_at_position(list, 1, 5);
    assert_equal(5, list->head->data, "Element inserted at position 1");
    assert_equal(5, list->count, "Count is 5");
    
    /* Insert invalid position */
    result = ll_insert_at_position(list, 100, 999);
    assert_equal(-1, result, "Invalid position returns -1");
    
    ll_free_list(&list);
}

/**
 * test_delete_beginning()
 * Tests: Delete from beginning
 */
void test_delete_beginning(void) {
    printf("\n─── Test Group: DELETE FROM BEGINNING ───\n");
    
    LinkedList *list = ll_create_list();
    ll_insert_end(list, 10);
    ll_insert_end(list, 20);
    ll_insert_end(list, 30);
    
    /* Delete first element */
    int deleted = ll_delete_beginning(list);
    assert_equal(10, deleted, "Deleted element is 10");
    assert_equal(2, list->count, "Count decreased to 2");
    assert_equal(20, list->head->data, "Head is now 20");
    
    /* Delete next */
    deleted = ll_delete_beginning(list);
    assert_equal(20, deleted, "Deleted element is 20");
    assert_equal(30, list->head->data, "Head is now 30");
    
    /* Delete last */
    deleted = ll_delete_beginning(list);
    assert_equal(30, deleted, "Deleted element is 30");
    assert_true(ll_is_empty(list), "List is now empty");
    
    ll_free_list(&list);
}

/**
 * test_delete_end()
 * Tests: Delete from end
 */
void test_delete_end(void) {
    printf("\n─── Test Group: DELETE FROM END ───\n");
    
    LinkedList *list = ll_create_list();
    ll_insert_end(list, 10);
    ll_insert_end(list, 20);
    ll_insert_end(list, 30);
    
    /* Delete last element */
    int deleted = ll_delete_end(list);
    assert_equal(30, deleted, "Deleted element is 30");
    assert_equal(2, list->count, "Count decreased to 2");
    assert_equal(20, list->head->next->data, "Last element is now 20");
    
    /* Delete next */
    deleted = ll_delete_end(list);
    assert_equal(20, deleted, "Deleted element is 20");
    assert_equal(10, list->head->data, "Only 10 remains");
    
    /* Delete last remaining */
    deleted = ll_delete_end(list);
    assert_equal(10, deleted, "Deleted element is 10");
    assert_true(ll_is_empty(list), "List is now empty");
    
    ll_free_list(&list);
}

/**
 * test_delete_at_position()
 * Tests: Delete at specific position
 */
void test_delete_at_position(void) {
    printf("\n─── Test Group: DELETE AT POSITION ───\n");
    
    LinkedList *list = ll_create_list();
    ll_insert_end(list, 10);
    ll_insert_end(list, 20);
    ll_insert_end(list, 30);
    ll_insert_end(list, 40);
    
    /* Delete at position 2 */
    int deleted = ll_delete_at_position(list, 2);
    assert_equal(20, deleted, "Deleted element at position 2 is 20");
    assert_equal(3, list->count, "Count is 3");
    assert_equal(30, list->head->next->data, "Position 2 is now 30");
    
    /* Delete invalid position */
    deleted = ll_delete_at_position(list, 100);
    assert_equal(-1, deleted, "Invalid position returns -1");
    
    ll_free_list(&list);
}

/**
 * test_search()
 * Tests: Search element in list
 */
void test_search(void) {
    printf("\n─── Test Group: SEARCH OPERATIONS ───\n");
    
    LinkedList *list = ll_create_list();
    ll_insert_end(list, 15);
    ll_insert_end(list, 8);
    ll_insert_end(list, 42);
    ll_insert_end(list, 23);
    
    /* Search existing elements */
    int pos = ll_search(list, 15);
    assert_equal(1, pos, "Element 15 found at position 1");
    
    pos = ll_search(list, 42);
    assert_equal(3, pos, "Element 42 found at position 3");
    
    pos = ll_search(list, 23);
    assert_equal(4, pos, "Element 23 found at position 4");
    
    /* Search non-existing element */
    pos = ll_search(list, 999);
    assert_equal(-1, pos, "Element 999 not found");
    
    ll_free_list(&list);
}

/**
 * test_count()
 * Tests: Count nodes in list
 */
void test_count(void) {
    printf("\n─── Test Group: COUNT OPERATIONS ───\n");
    
    LinkedList *list = ll_create_list();
    assert_equal(0, ll_count(list), "Empty list count is 0");
    
    ll_insert_end(list, 1);
    assert_equal(1, ll_count(list), "Count is 1 after first insert");
    
    for (int i = 2; i <= 5; i++) {
        ll_insert_end(list, i);
    }
    assert_equal(5, ll_count(list), "Count is 5 after 5 inserts");
    
    ll_delete_beginning(list);
    assert_equal(4, ll_count(list), "Count is 4 after delete");
    
    ll_free_list(&list);
}

/**
 * test_reverse()
 * Tests: Reverse the linked list
 */
void test_reverse(void) {
    printf("\n─── Test Group: REVERSE OPERATIONS ───\n");
    
    LinkedList *list = ll_create_list();
    ll_insert_end(list, 10);
    ll_insert_end(list, 20);
    ll_insert_end(list, 30);
    
    /* Reverse list */
    int result = ll_reverse(list);
    assert_equal(0, result, "Reverse returns 0");
    assert_equal(30, list->head->data, "Head is now 30");
    assert_equal(20, list->head->next->data, "Second is 20");
    assert_equal(10, list->head->next->next->data, "Third is 10");
    
    /* Reverse again */
    ll_reverse(list);
    assert_equal(10, list->head->data, "Head is back to 10");
    
    /* Single element list */
    LinkedList *single = ll_create_list();
    ll_insert_end(single, 42);
    result = ll_reverse(single);
    assert_equal(0, result, "Single element reverse succeeds");
    assert_equal(42, single->head->data, "Single element unchanged");
    
    ll_free_list(&list);
    ll_free_list(&single);
}

/**
 * test_sort()
 * Tests: Sort the linked list
 */
void test_sort(void) {
    printf("\n─── Test Group: SORT OPERATIONS ───\n");
    
    LinkedList *list = ll_create_list();
    ll_insert_end(list, 50);
    ll_insert_end(list, 15);
    ll_insert_end(list, 40);
    ll_insert_end(list, 8);
    ll_insert_end(list, 25);
    
    /* Sort list */
    int result = ll_sort(list);
    assert_equal(0, result, "Sort returns 0");
    assert_equal(8, list->head->data, "First element is 8");
    assert_equal(15, list->head->next->data, "Second element is 15");
    assert_equal(25, list->head->next->next->data, "Third element is 25");
    assert_equal(40, list->head->next->next->next->data, "Fourth is 40");
    assert_equal(50, list->head->next->next->next->next->data, "Last is 50");
    
    ll_free_list(&list);
}

/**
 * test_detect_loop()
 * Tests: Detect loop in linked list
 */
void test_detect_loop(void) {
    printf("\n─── Test Group: LOOP DETECTION ───\n");
    
    LinkedList *list = ll_create_list();
    ll_insert_end(list, 10);
    ll_insert_end(list, 20);
    ll_insert_end(list, 30);
    
    /* No loop initially */
    int has_loop = ll_detect_loop(list);
    assert_equal(0, has_loop, "No loop detected in normal list");
    
    /* Create a loop manually */
    Node *last = list->head;
    while (last->next != NULL) {
        last = last->next;
    }
    last->next = list->head;  /* Create loop back to beginning */
    
    has_loop = ll_detect_loop(list);
    assert_equal(1, has_loop, "Loop detected when created");
    
    /* Clean up without freeing (to avoid infinite loop) */
    last->next = NULL;  /* Break loop before freeing */
    ll_free_list(&list);
}

/**
 * test_get_middle()
 * Tests: Find middle element
 */
void test_get_middle(void) {
    printf("\n─── Test Group: GET MIDDLE OPERATIONS ───\n");
    
    LinkedList *list = ll_create_list();
    
    /* Single element */
    ll_insert_end(list, 10);
    assert_equal(10, ll_get_middle(list), "Middle of single element is itself");
    
    /* Two elements */
    ll_insert_end(list, 20);
    assert_equal(20, ll_get_middle(list), "Middle of two elements is second");
    
    /* Three elements */
    ll_insert_end(list, 30);
    assert_equal(20, ll_get_middle(list), "Middle of [10,20,30] is 20");
    
    /* Five elements */
    ll_insert_end(list, 40);
    ll_insert_end(list, 50);
    assert_equal(30, ll_get_middle(list), "Middle of [10,20,30,40,50] is 30");
    
    ll_free_list(&list);
}

/**
 * test_edge_cases()
 * Tests: Edge cases and error conditions
 */
void test_edge_cases(void) {
    printf("\n─── Test Group: EDGE CASES ───\n");
    
    LinkedList *list = ll_create_list();
    
    /* Operations on empty list */
    assert_true(ll_is_empty(list), "Empty list is empty");
    assert_equal(0, ll_count(list), "Empty list count is 0");
    
    /* Single element operations */
    ll_insert_end(list, 42);
    assert_true(!ll_is_empty(list), "Single element list is not empty");
    
    int deleted = ll_delete_end(list);
    assert_equal(42, deleted, "Delete end of single element works");
    assert_true(ll_is_empty(list), "List is empty again");
    
    /* Multiple insertions and deletions */
    for (int i = 1; i <= 10; i++) {
        ll_insert_end(list, i);
    }
    assert_equal(10, ll_count(list), "10 elements inserted");
    
    for (int i = 1; i <= 5; i++) {
        ll_delete_beginning(list);
    }
    assert_equal(5, ll_count(list), "5 elements deleted");
    
    ll_free_list(&list);
}

/**
 * main()
 * Run all test cases
 */
int main(void) {
    test_init();
    
    printf("\n╔════════════════════════════════════════════════╗\n");
    printf("║   LINKED LIST - COMPREHENSIVE TEST SUITE     ║\n");
    printf("╚════════════════════════════════════════════════╝\n");
    
    /* Run all test groups */
    test_create_list();
    test_insert_beginning();
    test_insert_end();
    test_insert_at_position();
    test_delete_beginning();
    test_delete_end();
    test_delete_at_position();
    test_search();
    test_count();
    test_reverse();
    test_sort();
    test_detect_loop();
    test_get_middle();
    test_edge_cases();
    
    test_cleanup();
    
    /* Return exit code based on failures */
    return (failed_tests == 0) ? 0 : 1;
}
