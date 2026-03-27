#ifndef STR_OPS_H
#define STR_OPS_H

#include <stddef.h>
#include <stdint.h>

/* Error codes for string operations */
typedef enum {
    STR_OK = 0,
    STR_ERR_NULL_PTR = -1,
    STR_ERR_EMPTY = -2,
    STR_ERR_ALLOC = -3,
    STR_ERR_INVALID_INPUT = -4,
    STR_ERR_OVERFLOW = -5
} str_error_t;

/* Maximum safe input size */
#define MAX_INPUT_SIZE 4096
#define MAX_STR_LEN 8192

/**
 * Manually calculate string length (no stdlib strlen)
 * @param str Input string (may be NULL)
 * @return Length of string, or -1 if NULL
 */
int32_t my_strlen(const char *str);

/**
 * Manually convert string to integer (no stdlib atoi)
 * @param str Input string
 * @param out Pointer to store result
 * @return STR_OK on success, error code otherwise
 */
str_error_t my_atoi(const char *str, int32_t *out);

/**
 * Reverse a null-terminated string in-place
 * @param str Input string (will be reversed in-place)
 * @return STR_OK on success, error code otherwise
 */
str_error_t reverse_string(char *str);

/**
 * Reverse words in a sentence (allocated copy)
 * @param sentence Input sentence
 * @param out Pointer to store reversed copy (caller must free)
 * @return STR_OK on success, error code otherwise
 */
str_error_t reverse_words(const char *sentence, char **out);

/**
 * Print error message for error code
 * @param err Error code
 */
void print_error(str_error_t err);

#endif /* STR_OPS_H */
