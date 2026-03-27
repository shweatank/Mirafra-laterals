/**
 * test_runner.h
 * 
 * Simple test framework for linked list testing.
 * Provides macros for assertions and test reporting.
 */

#ifndef TEST_RUNNER_H
#define TEST_RUNNER_H

#include <stdio.h>
#include <stdlib.h>

/* Global test counters */
extern int tests_passed;
extern int tests_failed;

/**
 * ASSERT - Assert that a condition is true.
 * 
 * Prints [PASS] if condition is true, [FAIL] if false.
 * Automatically increments test counters.
 */
#define ASSERT(condition, test_name) \
    do { \
        if (condition) { \
            printf("[PASS] %s\n", test_name); \
            tests_passed++; \
        } else { \
            printf("[FAIL] %s\n", test_name); \
            tests_failed++; \
        } \
    } while (0)

/**
 * ASSERT_EQ - Assert that two integers are equal.
 * 
 * Prints [PASS] if equal, [FAIL] with actual/expected values if not.
 */
#define ASSERT_EQ(actual, expected, test_name) \
    do { \
        if ((actual) == (expected)) { \
            printf("[PASS] %s\n", test_name); \
            tests_passed++; \
        } else { \
            printf("[FAIL] %s - Expected %d, got %d\n", test_name, (expected), (actual)); \
            tests_failed++; \
        } \
    } while (0)

/**
 * ASSERT_NEQ - Assert that two integers are NOT equal.
 */
#define ASSERT_NEQ(actual, unexpected, test_name) \
    do { \
        if ((actual) != (unexpected)) { \
            printf("[PASS] %s\n", test_name); \
            tests_passed++; \
        } else { \
            printf("[FAIL] %s - Expected NOT %d, but got %d\n", test_name, (unexpected), (actual)); \
            tests_failed++; \
        } \
    } while (0)

/**
 * ASSERT_NULL - Assert that a pointer is NULL.
 */
#define ASSERT_NULL(ptr, test_name) \
    do { \
        if ((ptr) == NULL) { \
            printf("[PASS] %s\n", test_name); \
            tests_passed++; \
        } else { \
            printf("[FAIL] %s - Expected NULL but got non-NULL pointer\n", test_name); \
            tests_failed++; \
        } \
    } while (0)

/**
 * ASSERT_NOT_NULL - Assert that a pointer is NOT NULL.
 */
#define ASSERT_NOT_NULL(ptr, test_name) \
    do { \
        if ((ptr) != NULL) { \
            printf("[PASS] %s\n", test_name); \
            tests_passed++; \
        } else { \
            printf("[FAIL] %s - Expected non-NULL pointer but got NULL\n", test_name); \
            tests_failed++; \
        } \
    } while (0)

/**
 * TEST_HEADER - Print test header with timestamp.
 */
#define TEST_HEADER(name) \
    do { \
        printf("\n============================\n"); \
        printf("Running: %s\n", name); \
        printf("============================\n"); \
    } while (0)

/**
 * TEST_FOOTER - Print test footer with summary.
 */
#define TEST_FOOTER() \
    do { \
        printf("============================\n"); \
        printf("Results: %d passed, %d failed\n", tests_passed, tests_failed); \
        printf("============================\n"); \
    } while (0)

#endif /* TEST_RUNNER_H */
