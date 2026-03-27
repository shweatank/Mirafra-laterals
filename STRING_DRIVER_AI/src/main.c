#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "str_ops.h"

/* Function prototypes */
static void print_menu(void);
static void handle_reverse_string(void);
static void handle_string_to_int(void);
static void handle_strlen(void);
static void handle_reverse_words(void);
static char *read_input(const char *prompt, size_t max_size);

/**
 * Print the main menu
 */
static void print_menu(void)
{
    printf("\n=== String Operations Utility ===\n");
    printf("1. Reverse a string\n");
    printf("2. Convert string to integer\n");
    printf("3. Get string length\n");
    printf("4. Reverse words in sentence\n");
    printf("5. Exit\n");
    printf("Select operation (1-5): ");
}

/**
 * Safely read input from stdin
 * @param prompt User prompt text
 * @param max_size Maximum input size
 * @return Allocated string (caller must free), or NULL on error
 */
static char *read_input(const char *prompt, size_t max_size)
{
    if (!prompt || max_size == 0 || max_size > MAX_INPUT_SIZE)
        return NULL;
    
    printf("%s", prompt);
    fflush(stdout);
    
    char *buffer = (char *)malloc(max_size + 1);
    if (!buffer) {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }
    
    memset(buffer, 0, max_size + 1);
    
    if (!fgets(buffer, (int)max_size + 1, stdin)) {
        free(buffer);
        return NULL;
    }
    
    /* Remove trailing newline */
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n')
        buffer[len - 1] = '\0';
    
    return buffer;
}

/**
 * Handler for reverse string operation
 */
static void handle_reverse_string(void)
{
    char *str = read_input("Enter string to reverse: ", MAX_INPUT_SIZE);
    if (!str)
        return;
    
    str_error_t err = reverse_string(str);
    if (err == STR_OK) {
        printf("Reversed: %s\n", str);
    } else {
        print_error(err);
    }
    
    free(str);
}

/**
 * Handler for string to integer conversion
 */
static void handle_string_to_int(void)
{
    char *str = read_input("Enter number string: ", MAX_INPUT_SIZE);
    if (!str)
        return;
    
    int32_t result = 0;
    str_error_t err = my_atoi(str, &result);
    if (err == STR_OK) {
        printf("Result: %d\n", result);
    } else {
        print_error(err);
    }
    
    free(str);
}

/**
 * Handler for string length operation
 */
static void handle_strlen(void)
{
    char *str = read_input("Enter string: ", MAX_INPUT_SIZE);
    if (!str)
        return;
    
    int32_t len = my_strlen(str);
    if (len >= 0) {
        printf("Length: %d\n", len);
    } else {
        print_error(STR_ERR_NULL_PTR);
    }
    
    free(str);
}

/**
 * Handler for reverse words operation
 */
static void handle_reverse_words(void)
{
    char *sentence = read_input("Enter sentence: ", MAX_INPUT_SIZE);
    if (!sentence)
        return;
    
    char *result = NULL;
    str_error_t err = reverse_words(sentence, &result);
    if (err == STR_OK) {
        printf("Reversed: %s\n", result);
        free(result);
    } else {
        print_error(err);
    }
    
    free(sentence);
}

/**
 * Main entry point with interactive loop
 */
int main(void)
{
    printf("String Operations Utility v1.0\n");
    printf("A production-grade C11 string manipulation tool\n");
    
    int running = 1;
    
    while (running) {
        print_menu();
        
        char *choice = read_input("", 10);
        if (!choice)
            break;
        
        if (strlen(choice) == 0) {
            free(choice);
            continue;
        }
        
        switch (choice[0]) {
        case '1':
            handle_reverse_string();
            break;
        case '2':
            handle_string_to_int();
            break;
        case '3':
            handle_strlen();
            break;
        case '4':
            handle_reverse_words();
            break;
        case '5':
            printf("Exiting. Goodbye!\n");
            running = 0;
            break;
        default:
            printf("Invalid choice. Please select 1-5.\n");
            break;
        }
        
        free(choice);
    }
    
    return 0;
}
