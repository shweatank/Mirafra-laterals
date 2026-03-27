/*
 * ============================================================================
 * FILE: main.c
 * DESC: Main program demonstrating all linked list operations
 * AUTH: System Programmer
 * DATE: March 27, 2026
 * ============================================================================
 */

#include "../include/linkedlist.h"

/* Function prototypes for demo functions */
void print_menu(void);
void demo_basic_operations(void);
void demo_insertion_operations(void);
void demo_deletion_operations(void);
void demo_search_operations(void);
void demo_advanced_operations(void);

/**
 * print_menu()
 * Displays the main menu options
 */
void print_menu(void) {
    printf("\n");
    printf("╔════════════════════════════════════════════════╗\n");
    printf("║       LINKED LIST ALL OPERATIONS DEMO         ║\n");
    printf("╠════════════════════════════════════════════════╣\n");
    printf("║ 1. Demo - Basic Operations (Create, Insert)  ║\n");
    printf("║ 2. Demo - Insertion at Different Positions   ║\n");
    printf("║ 3. Demo - Deletion Operations                ║\n");
    printf("║ 4. Demo - Search & Count Operations          ║\n");
    printf("║ 5. Demo - Advanced Operations                ║\n");
    printf("║ 6. Exit                                       ║\n");
    printf("╚════════════════════════════════════════════════╝\n");
    printf("Enter your choice: ");
}

/**
 * demo_basic_operations()
 * Demonstrates: Create List, Insert Beginning, Insert End, Display
 */
void demo_basic_operations(void) {
    printf("\n─── DEMO 1: BASIC OPERATIONS ───\n");
    
    /* Create a new linked list */
    LinkedList *list = ll_create_list();
    printf("✓ Created empty linked list\n");
    
    /* Insert at beginning */
    ll_insert_beginning(list, 10);
    ll_insert_beginning(list, 5);
    printf("✓ Inserted 5, 10 at beginning\n");
    
    /* Insert at end */
    ll_insert_end(list, 15);
    ll_insert_end(list, 20);
    printf("✓ Inserted 15, 20 at end\n");
    
    /* Display list */
    printf("─ Current list:\n  ");
    ll_display(list);
    
    /* Count nodes */
    int count = ll_count(list);
    printf("─ Total nodes: %d\n", count);
    
    /* Clean up */
    ll_free_list(&list);
    printf("✓ List cleared and memory freed\n");
}

/**
 * demo_insertion_operations()
 * Demonstrates: Insert at specific positions
 */
void demo_insertion_operations(void) {
    printf("\n─── DEMO 2: INSERTION AT DIFFERENT POSITIONS ───\n");
    
    LinkedList *list = ll_create_list();
    printf("✓ Created empty linked list\n");
    
    /* Build initial list */
    ll_insert_end(list, 10);
    ll_insert_end(list, 20);
    ll_insert_end(list, 40);
    ll_insert_end(list, 50);
    printf("✓ Initial list created: ");
    ll_display(list);
    
    /* Insert at position 2 */
    printf("─ Inserting 15 at position 2:\n");
    ll_insert_at_position(list, 2, 15);
    printf("  ");
    ll_display(list);
    
    /* Insert at position 3 */
    printf("─ Inserting 25 at position 3:\n");
    ll_insert_at_position(list, 3, 25);
    printf("  ");
    ll_display(list);
    
    /* Insert at position 6 (between 40 and 50) */
    printf("─ Inserting 45 at position 6:\n");
    ll_insert_at_position(list, 6, 45);
    printf("  ");
    ll_display(list);
    
    /* Clean up */
    ll_free_list(&list);
    printf("✓ List cleared and memory freed\n");
}

/**
 * demo_deletion_operations()
 * Demonstrates: Delete from beginning, end, and specific position
 */
void demo_deletion_operations(void) {
    printf("\n─── DEMO 3: DELETION OPERATIONS ───\n");
    
    LinkedList *list = ll_create_list();
    printf("✓ Created empty linked list\n");
    
    /* Build initial list */
    for (int i = 10; i <= 50; i += 10) {
        ll_insert_end(list, i);
    }
    printf("✓ Initial list: ");
    ll_display(list);
    
    /* Delete from beginning */
    printf("─ Delete from beginning:\n");
    int deleted = ll_delete_beginning(list);
    printf("  Deleted: %d\n  ", deleted);
    ll_display(list);
    
    /* Delete from end */
    printf("─ Delete from end:\n");
    deleted = ll_delete_end(list);
    printf("  Deleted: %d\n  ", deleted);
    ll_display(list);
    
    /* Delete at position 2 */
    printf("─ Delete at position 2:\n");
    deleted = ll_delete_at_position(list, 2);
    printf("  Deleted: %d\n  ", deleted);
    ll_display(list);
    
    /* Clean up */
    ll_free_list(&list);
    printf("✓ List cleared and memory freed\n");
}

/**
 * demo_search_operations()
 * Demonstrates: Search element, Count nodes, Check empty
 */
void demo_search_operations(void) {
    printf("\n─── DEMO 4: SEARCH & COUNT OPERATIONS ───\n");
    
    LinkedList *list = ll_create_list();
    printf("✓ Created empty linked list\n");
    
    /* Check if empty */
    if (ll_is_empty(list)) {
        printf("✓ List is empty\n");
    }
    
    /* Build list */
    int values[] = {15, 8, 42, 23, 16, 4, 99};
    int size = sizeof(values) / sizeof(values[0]);
    
    for (int i = 0; i < size; i++) {
        ll_insert_end(list, values[i]);
    }
    printf("✓ Created list with %d elements: ", size);
    ll_display(list);
    
    /* Search for elements */
    printf("─ Search operations:\n");
    int search_values[] = {23, 99, 100, 8};
    int search_size = sizeof(search_values) / sizeof(search_values[0]);
    
    for (int i = 0; i < search_size; i++) {
        int pos = ll_search(list, search_values[i]);
        if (pos != -1) {
            printf("  ✓ Element %d found at position %d\n", search_values[i], pos);
        } else {
            printf("  ✗ Element %d not found\n", search_values[i]);
        }
    }
    
    /* Count nodes */
    int count = ll_count(list);
    printf("─ Total nodes in list: %d\n", count);
    
    /* Get middle element */
    int middle = ll_get_middle(list);
    printf("─ Middle element: %d\n", middle);
    
    /* Clean up */
    ll_free_list(&list);
    printf("✓ List cleared and memory freed\n");
}

/**
 * demo_advanced_operations()
 * Demonstrates: Reverse, Sort, Detect Loop
 */
void demo_advanced_operations(void) {
    printf("\n─── DEMO 5: ADVANCED OPERATIONS ───\n");
    
    LinkedList *list = ll_create_list();
    printf("✓ Created empty linked list\n");
    
    /* Build initial list */
    int values[] = {50, 15, 40, 8, 25};
    int size = sizeof(values) / sizeof(values[0]);
    
    for (int i = 0; i < size; i++) {
        ll_insert_end(list, values[i]);
    }
    printf("✓ Initial list: ");
    ll_display(list);
    
    /* Detect loop (should be none) */
    printf("─ Loop detection (before creating loop):\n");
    int loop = ll_detect_loop(list);
    if (loop == 1) {
        printf("  ✓ Loop detected in list\n");
    } else if (loop == 0) {
        printf("  ✓ No loop detected (as expected)\n");
    }
    
    /* Sort list */
    printf("─ Sorting list in ascending order:\n");
    ll_sort(list);
    printf("  ");
    ll_display(list);
    
    /* Reverse list */
    printf("─ Reversing list:\n");
    ll_reverse(list);
    printf("  ");
    ll_display(list);
    
    /* Display reverse using recursive function */
    printf("─ Display in reverse order (using recursion):\n  Reverse: ");
    ll_display_reverse(list->head);
    printf(" -> NULL\n");
    
    /* Clean up */
    ll_free_list(&list);
    printf("✓ List cleared and memory freed\n");
}

/**
 * main()
 * Main program entry point with interactive menu
 */
int main(void) {
    printf("\n╔════════════════════════════════════════════════╗\n");
    printf("║    LINKED LIST - COMPLETE IMPLEMENTATION     ║\n");
    printf("║              All Operations Demo             ║\n");
    printf("╚════════════════════════════════════════════════╝\n");
    
    int choice = 0;
    
    while (1) {
        print_menu();
        scanf("%d", &choice);
        
        switch (choice) {
            case 1:
                demo_basic_operations();
                break;
                
            case 2:
                demo_insertion_operations();
                break;
                
            case 3:
                demo_deletion_operations();
                break;
                
            case 4:
                demo_search_operations();
                break;
                
            case 5:
                demo_advanced_operations();
                break;
                
            case 6:
                printf("\n✓ Thank you for using Linked List Demo!\n");
                printf("✓ Exiting...\n\n");
                return 0;
                
            default:
                printf("✗ Invalid choice. Please enter 1-6.\n");
        }
    }
    
    return 0;
}
