#include "str_ops.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

/**
 * Manually calculate string length
 * @param str Input string (may be NULL)
 * @return Length of string, or -1 if NULL
 */
int32_t my_strlen(const char *str)
{
    if (!str)
        return -1;
    
    int32_t len = 0;
    while (str[len] != '\0') {
        if (len >= MAX_STR_LEN)
            return -1; /* Prevent integer overflow */
        len++;
    }
    return len;
}

/**
 * Manually convert string to integer with overflow protection
 * Handles: leading whitespace, optional +/- sign, decimal digits
 * @param str Input string
 * @param out Pointer to store result
 * @return STR_OK on success, error code otherwise
 */
str_error_t my_atoi(const char *str, int32_t *out)
{
    if (!str || !out)
        return STR_ERR_NULL_PTR;
    
    int32_t len = my_strlen(str);
    if (len == 0)
        return STR_ERR_EMPTY;
    
    /* Skip leading whitespace */
    size_t idx = 0;
    while (idx < (size_t)len && isspace((unsigned char)str[idx]))
        idx++;
    
    if (idx >= (size_t)len)
        return STR_ERR_INVALID_INPUT; /* All whitespace */
    
    /* Handle sign */
    int32_t sign = 1;
    if (str[idx] == '-') {
        sign = -1;
        idx++;
    } else if (str[idx] == '+') {
        idx++;
    }
    
    if (idx >= (size_t)len || !isdigit((unsigned char)str[idx]))
        return STR_ERR_INVALID_INPUT;
    
    /* Convert digits with overflow check */
    int32_t result = 0;
    const int32_t INT32_MAX_VAL = 2147483647;
    const int32_t INT32_MIN_VAL = -2147483648;
    
    while (idx < (size_t)len && isdigit((unsigned char)str[idx])) {
        int32_t digit = str[idx] - '0';
        
        /* Check for overflow: result * 10 + digit */
        if (sign > 0) {
            if (result > INT32_MAX_VAL / 10)
                return STR_ERR_OVERFLOW;
            if (result == INT32_MAX_VAL / 10 && digit > 7)
                return STR_ERR_OVERFLOW;
        } else {
            if (result < INT32_MIN_VAL / 10)
                return STR_ERR_OVERFLOW;
            if (result == INT32_MIN_VAL / 10 && digit > 8)
                return STR_ERR_OVERFLOW;
        }
        
        result = result * 10 + digit;
        idx++;
    }
    
    /* Check for trailing non-whitespace characters */
    while (idx < (size_t)len) {
        if (!isspace((unsigned char)str[idx]))
            return STR_ERR_INVALID_INPUT;
        idx++;
    }
    
    *out = result * sign;
    return STR_OK;
}

/**
 * Reverse a null-terminated string in-place
 * @param str Input string (will be reversed in-place)
 * @return STR_OK on success, error code otherwise
 */
str_error_t reverse_string(char *str)
{
    if (!str)
        return STR_ERR_NULL_PTR;
    
    int32_t len = my_strlen(str);
    if (len <= 0)
        return STR_ERR_EMPTY;
    
    int32_t left = 0;
    int32_t right = len - 1;
    
    while (left < right) {
        /* Swap characters */
        char tmp = str[left];
        str[left] = str[right];
        str[right] = tmp;
        
        left++;
        right--;
    }
    
    return STR_OK;
}

/**
 * Reverse words in a sentence (allocated copy)
 * Reverses the order of words, not individual characters
 * Example: "hello world" -> "world hello"
 * @param sentence Input sentence
 * @param out Pointer to store reversed copy (caller must free)
 * @return STR_OK on success, error code otherwise
 */
str_error_t reverse_words(const char *sentence, char **out)
{
    if (!sentence || !out)
        return STR_ERR_NULL_PTR;
    
    int32_t len = my_strlen(sentence);
    if (len <= 0)
        return STR_ERR_EMPTY;
    
    /* Allocate memory for result and temp storage */
    char *result = (char *)malloc((size_t)len + 1);
    if (!result)
        return STR_ERR_ALLOC;
    
    char *temp = (char *)malloc((size_t)len + 1);
    if (!temp) {
        free(result);
        return STR_ERR_ALLOC;
    }
    
    memset(result, 0, (size_t)len + 1);
    memset(temp, 0, (size_t)len + 1);
    
    /* Extract words into array */
    #define MAX_WORDS 256
    int32_t word_starts[MAX_WORDS];
    int32_t word_lens[MAX_WORDS];
    int32_t word_count = 0;
    
    int32_t i = 0;
    while (i < len && word_count < MAX_WORDS) {
        /* Skip whitespace */
        while (i < len && isspace((unsigned char)sentence[i]))
            i++;
        
        if (i >= len)
            break;
        
        /* Found start of word */
        word_starts[word_count] = i;
        int32_t word_start = i;
        
        /* Find end of word */
        while (i < len && !isspace((unsigned char)sentence[i]))
            i++;
        
        word_lens[word_count] = i - word_start;
        word_count++;
    }
    
    /* Reconstruct in reverse word order */
    int32_t pos = 0;
    for (int32_t w = word_count - 1; w >= 0; w--) {
        /* Copy word */
        memcpy(result + pos, sentence + word_starts[w], (size_t)word_lens[w]);
        pos += word_lens[w];
        
        /* Add space if not last word */
        if (w > 0) {
            result[pos++] = ' ';
        }
    }
    
    result[pos] = '\0';
    free(temp);
    *out = result;
    return STR_OK;
}

/**
 * Print error message for error code
 * @param err Error code
 */
void print_error(str_error_t err)
{
    switch (err) {
    case STR_OK:
        printf("No error\n");
        break;
    case STR_ERR_NULL_PTR:
        printf("Error: NULL pointer passed\n");
        break;
    case STR_ERR_EMPTY:
        printf("Error: Empty string\n");
        break;
    case STR_ERR_ALLOC:
        printf("Error: Memory allocation failed\n");
        break;
    case STR_ERR_INVALID_INPUT:
        printf("Error: Invalid input format\n");
        break;
    case STR_ERR_OVERFLOW:
        printf("Error: Integer overflow\n");
        break;
    default:
        printf("Error: Unknown error (%d)\n", err);
        break;
    }
}
