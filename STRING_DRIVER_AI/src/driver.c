/**
 * STRING_DRIVER_AI - Automated Test Driver
 * Comprehensive test suite for string operations utilities
 * Generates detailed test logs and validates all modules
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "str_ops.h"

#define TEST_RESULT_LOG "test_results.log"
#define MAX_TEST_CASES 15

/* Test result tracking */
typedef struct {
    int total;
    int passed;
    int failed;
    int skipped;
} test_stats_t;

/* Test case structure */
typedef struct {
    int id;
    const char *description;
    const char *operation;
} test_case_t;

/* Global test statistics */
static test_stats_t stats = {0, 0, 0, 0};
static FILE *log_file = NULL;

/**
 * Initialize test logging
 */
static int init_logging(void)
{
    log_file = fopen(TEST_RESULT_LOG, "w");
    if (!log_file) {
        fprintf(stderr, "ERROR: Cannot open log file: %s\n", TEST_RESULT_LOG);
        return -1;
    }
    
    time_t now = time(NULL);
    char time_str[64];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", localtime(&now));
    
    fprintf(log_file, "========================================\n");
    fprintf(log_file, "STRING_DRIVER_AI - Test Suite Log\n");
    fprintf(log_file, "Generated: %s\n", time_str);
    fprintf(log_file, "========================================\n\n");
    
    printf("\n[LOG] Test driver initialized at %s\n", time_str);
    return 0;
}

/**
 * Close test logging
 */
static void close_logging(void)
{
    if (!log_file) return;
    
    fprintf(log_file, "\n========================================\n");
    fprintf(log_file, "Test Summary\n");
    fprintf(log_file, "========================================\n");
    fprintf(log_file, "Total Tests:    %d\n", stats.total);
    fprintf(log_file, "Passed:         %d\n", stats.passed);
    fprintf(log_file, "Failed:         %d\n", stats.failed);
    fprintf(log_file, "Skipped:        %d\n", stats.skipped);
    fprintf(log_file, "Success Rate:   %.1f%%\n", 
            stats.total > 0 ? (100.0 * stats.passed / stats.total) : 0);
    fprintf(log_file, "========================================\n");
    
    fclose(log_file);
}

/**
 * Log test result
 */
static void log_result(int test_id, const char *description, 
                      int passed, const char *details)
{
    const char *status = passed ? "PASS" : "FAIL";
    
    if (log_file) {
        fprintf(log_file, "[%s] Test %d: %s\n", status, test_id, description);
        if (details) {
            fprintf(log_file, "    Details: %s\n", details);
        }
        fprintf(log_file, "\n");
    }
    
    printf("[%s] Test %d: %s\n", status, test_id, description);
    
    stats.total++;
    if (passed) {
        stats.passed++;
    } else {
        stats.failed++;
    }
}

/**
 * Test 1: Reverse simple string
 */
static void test_reverse_simple(void)
{
    char str[] = "hello";
    char expected[] = "olleh";
    reverse_string(str);
    
    int passed = strcmp(str, expected) == 0;
    log_result(1, "Reverse simple string", passed, 
              passed ? "OK" : "Expected 'olleh', got result");
}

/**
 * Test 2: Reverse with spaces
 */
static void test_reverse_spaces(void)
{
    char str[] = "hello world";
    char expected[] = "dlrow olleh";
    reverse_string(str);
    
    int passed = strcmp(str, expected) == 0;
    log_result(2, "Reverse with spaces", passed,
              passed ? "OK" : "Space handling validated");
}

/**
 * Test 3: Reverse single character
 */
static void test_reverse_single(void)
{
    char str[] = "a";
    char expected[] = "a";
    reverse_string(str);
    
    int passed = strcmp(str, expected) == 0;
    log_result(3, "Reverse single character", passed, "OK");
}

/**
 * Test 4: String to int - basic positive
 */
static void test_atoi_positive(void)
{
    int32_t result;
    str_error_t err = my_atoi("123", &result);
    
    int passed = (err == STR_OK && result == 123);
    log_result(4, "Convert string to int (positive)", passed,
              passed ? "123 -> OK" : "Conversion error");
}

/**
 * Test 5: String to int - negative number
 */
static void test_atoi_negative(void)
{
    int32_t result;
    str_error_t err = my_atoi("-456", &result);
    
    int passed = (err == STR_OK && result == -456);
    log_result(5, "Convert string to int (negative)", passed,
              passed ? "-456 -> OK" : "Negative handling error");
}

/**
 * Test 6: String to int - with leading spaces
 */
static void test_atoi_spaces(void)
{
    int32_t result;
    str_error_t err = my_atoi("  789", &result);
    
    int passed = (err == STR_OK && result == 789);
    log_result(6, "Convert string to int (leading spaces)", passed,
              passed ? "789 -> OK" : "Space trimming error");
}

/**
 * Test 7: String length - basic string
 */
static void test_strlen_basic(void)
{
    int32_t len = my_strlen("hello");
    
    int passed = (len == 5);
    log_result(7, "String length (basic)", passed,
              passed ? "5 -> OK" : "Length calculation error");
}

/**
 * Test 8: String length - empty string
 */
static void test_strlen_empty(void)
{
    int32_t len = my_strlen("");
    
    int passed = (len == 0);
    log_result(8, "String length (empty)", passed, "OK");
}

/**
 * Test 9: String length - long string
 */
static void test_strlen_long(void)
{
    const char *str = "The quick brown fox jumps over the lazy dog";
    int32_t len = my_strlen(str);
    
    int passed = (len == 43);
    log_result(9, "String length (long string)", passed,
              passed ? "43 -> OK" : "Long string handling error");
}

/**
 * Test 10: Reverse words - two words
 */
static void test_reverse_words_two(void)
{
    char *result = NULL;
    str_error_t err = reverse_words("hello world", &result);
    
    int passed = (err == STR_OK && result && strcmp(result, "world hello") == 0);
    
    if (result) free(result);
    
    log_result(10, "Reverse words (two words)", passed,
              passed ? "OK" : "Word reversal error");
}

/**
 * Test 11: Reverse words - three words
 */
static void test_reverse_words_three(void)
{
    char *result = NULL;
    str_error_t err = reverse_words("one two three", &result);
    
    int passed = (err == STR_OK && result && strcmp(result, "three two one") == 0);
    
    if (result) free(result);
    
    log_result(11, "Reverse words (three words)", passed,
              passed ? "OK" : "Multiple word reversal error");
}

/**
 * Test 12: Integer overflow - maximum positive
 */
static void test_atoi_max_int(void)
{
    int32_t result;
    str_error_t err = my_atoi("2147483647", &result);
    
    int passed = (err == STR_OK && result == 2147483647);
    log_result(12, "Integer overflow (max positive)", passed,
              passed ? "OK (INT32_MAX)" : "Overflow detection error");
}

/**
 * Test 13: Integer overflow - overflow attempt
 */
static void test_atoi_overflow(void)
{
    int32_t result;
    str_error_t err = my_atoi("2147483648", &result);
    
    int passed = (err == STR_ERR_OVERFLOW);
    log_result(13, "Integer overflow (overflow detection)", passed,
              passed ? "Correctly detected overflow" : "Expected overflow error");
}

/**
 * Test 14: String to int - invalid input
 */
static void test_atoi_invalid(void)
{
    int32_t result;
    str_error_t err = my_atoi("12a34", &result);
    
    int passed = (err != STR_OK);
    log_result(14, "Convert string to int (invalid)", passed,
              passed ? "Correctly rejected invalid input" : "Should reject non-digits");
}

/**
 * Test 15: Null pointer handling
 */
static void test_null_handling(void)
{
    int32_t len = my_strlen(NULL);
    
    int passed = (len == -1);
    log_result(15, "Null pointer handling", passed,
              passed ? "OK" : "NULL check failed");
}

/**
 * Run all test cases
 */
static void run_all_tests(void)
{
    printf("\n========================================\n");
    printf("Running STRING_DRIVER_AI Test Suite\n");
    printf("========================================\n\n");
    
    test_reverse_simple();
    test_reverse_spaces();
    test_reverse_single();
    test_atoi_positive();
    test_atoi_negative();
    test_atoi_spaces();
    test_strlen_basic();
    test_strlen_empty();
    test_strlen_long();
    test_reverse_words_two();
    test_reverse_words_three();
    test_atoi_max_int();
    test_atoi_overflow();
    test_atoi_invalid();
    test_null_handling();
}

/**
 * Main entry point for test driver
 */
int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    
    if (init_logging() != 0) {
        return EXIT_FAILURE;
    }
    
    run_all_tests();
    close_logging();
    
    printf("\n========================================\n");
    printf("Test Summary:\n");
    printf("  Total Tests: %d\n", stats.total);
    printf("  Passed:      %d\n", stats.passed);
    printf("  Failed:      %d\n", stats.failed);
    printf("  Success Rate: %.1f%%\n",
           stats.total > 0 ? (100.0 * stats.passed / stats.total) : 0);
    printf("========================================\n");
    printf("Results saved to: %s\n\n", TEST_RESULT_LOG);
    
    return stats.failed > 0 ? EXIT_FAILURE : EXIT_SUCCESS;
}
