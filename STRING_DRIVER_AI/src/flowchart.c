#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gd.h>

/* Global image pointer for helper functions */
static gdImagePtr g_img = NULL;
static int g_black = 0;

/**
 * Draw a filled rectangle box
 */
static void draw_box(int x, int y, int w, int h, int color)
{
    gdImageFilledRectangle(g_img, x, y, x + w, y + h, color);
    gdImageRectangle(g_img, x, y, x + w, y + h, g_black);
}

/**
 * Draw an arrow from (x1,y1) to (x2,y2)
 */
static void draw_arrow(int x1, int y1, int x2, int y2)
{
    gdImageLine(g_img, x1, y1, x2, y2, g_black);
    /* Arrowhead for downward arrows */
    if (y2 > y1) {
        gdImageLine(g_img, x2, y2, x2 - 5, y2 - 10, g_black);
        gdImageLine(g_img, x2, y2, x2 + 5, y2 - 10, g_black);
    }
}

/**
 * Draw text at position (placeholder - GD text rendering varies by build)
 */
static void draw_text(int x, int y, const char *text, int color)
{
    (void)x;     /* Suppress unused parameter warning */
    (void)y;
    (void)text;
    (void)color;
    /* Text rendering depends on GD library build configuration */
    /* For production, use gdImageStringFT or similar */
}

/**
 * Generate a flowchart JPEG showing the program logic
 * Uses GD library for image generation
 * Output: flowchart.jpeg in current directory
 */
int main(void)
{
    /* Image dimensions */
    int width = 1000;
    int height = 1400;
    
    /* Create new image */
    g_img = gdImageCreateTrueColor(width, height);
    if (!g_img) {
        fprintf(stderr, "Failed to create image\n");
        return 1;
    }
    
    /* Allocate colors */
    int white = gdImageColorAllocate(g_img, 255, 255, 255);
    g_black = gdImageColorAllocate(g_img, 0, 0, 0);
    int green = gdImageColorAllocate(g_img, 0, 200, 0);
    int red = gdImageColorAllocate(g_img, 255, 0, 0);
    int yellow = gdImageColorAllocate(g_img, 200, 200, 0);
    int lightblue = gdImageColorAllocate(g_img, 173, 216, 230);
    
    /* Set background */
    gdImageFilledRectangle(g_img, 0, 0, width, height, white);
    
    /* TITLE */
    draw_text(300, 20, "String Operations Flowchart", g_black);
    
    /* START */
    int y_pos = 60;
    draw_box(400, y_pos, 200, 40, green);
    draw_text(430, y_pos + 10, "Program Start", g_black);
    
    /* Arrow down */
    draw_arrow(500, y_pos + 40, 500, y_pos + 60);
    
    /* MAIN MENU */
    y_pos += 80;
    draw_box(300, y_pos, 400, 60, lightblue);
    draw_text(350, y_pos + 10, "Display Menu:", g_black);
    draw_text(330, y_pos + 30, "1.Reverse  2.Atoi  3.Strlen  4.ReverseWords  5.Exit", g_black);
    
    /* Arrow down */
    draw_arrow(500, y_pos + 60, 500, y_pos + 80);
    
    /* DECISION: Choice */
    y_pos += 100;
    draw_box(400, y_pos, 200, 40, yellow);
    draw_text(430, y_pos + 10, "Read User Input", g_black);
    
    /* OPERATION 1: Reverse String */
    draw_arrow(300, y_pos + 20, 150, y_pos + 50);
    draw_box(50, y_pos + 50, 200, 60, lightblue);
    draw_text(80, y_pos + 60, "Reverse String", g_black);
    draw_text(85, y_pos + 75, "validate -> reverse", g_black);
    
    /* OPERATION 2: String to Int */
    draw_arrow(500, y_pos - 40, 500, y_pos - 20);
    draw_box(400, y_pos + 50, 200, 60, lightblue);
    draw_text(420, y_pos + 60, "String to Integer", g_black);
    draw_text(440, y_pos + 75, "parse digits", g_black);
    
    /* OPERATION 3: Strlen */
    draw_arrow(700, y_pos + 20, 850, y_pos + 50);
    draw_box(750, y_pos + 50, 200, 60, lightblue);
    draw_text(770, y_pos + 60, "Get String Length", g_black);
    draw_text(810, y_pos + 75, "count chars", g_black);
    
    /* OPERATION 4: Reverse Words */
    draw_arrow(300, y_pos + 40, 150, y_pos + 150);
    draw_box(50, y_pos + 150, 200, 60, lightblue);
    draw_text(70, y_pos + 160, "Reverse Words", g_black);
    draw_text(85, y_pos + 175, "swap word order", g_black);
    
    /* OPERATION 5: Exit */
    draw_arrow(700, y_pos + 40, 850, y_pos + 150);
    draw_box(750, y_pos + 150, 200, 60, red);
    draw_text(800, y_pos + 160, "EXIT", g_black);
    draw_text(785, y_pos + 175, "cleanup & exit", g_black);
    
    /* Convergence arrows */
    y_pos += 240;
    draw_arrow(150, y_pos - 30, 300, y_pos + 20);
    draw_arrow(500, y_pos - 80, 500, y_pos + 20);
    draw_arrow(850, y_pos - 30, 700, y_pos + 20);
    draw_arrow(150, y_pos - 100, 400, y_pos + 20);
    draw_arrow(850, y_pos - 100, 600, y_pos + 20);
    
    /* LOOP CHECK */
    draw_box(350, y_pos, 300, 60, yellow);
    draw_text(390, y_pos + 10, "Continue or Exit?", g_black);
    draw_text(370, y_pos + 30, "Exit -> Cleanup", g_black);
    
    /* Arrow to loop back */
    draw_arrow(350, y_pos + 30, 200, y_pos + 30);
    draw_arrow(200, y_pos + 30, 200, 100);
    draw_arrow(200, 100, 300, 100);
    draw_text(220, y_pos + 30, "loop", green);
    
    /* Arrow to end */
    draw_arrow(500, y_pos + 60, 500, y_pos + 100);
    
    /* END */
    y_pos += 140;
    draw_box(400, y_pos, 200, 40, red);
    draw_text(440, y_pos + 10, "Program End", g_black);
    
    /* Write to file */
    FILE *f = fopen("flowchart.jpeg", "wb");
    if (!f) {
        fprintf(stderr, "Failed to open flowchart.jpeg for writing\n");
        gdImageDestroy(g_img);
        return 1;
    }
    
    gdImageJpeg(g_img, f, 90);
    fclose(f);
    
    /* Clean up */
    gdImageDestroy(g_img);
    
    printf("Flowchart generated: flowchart.jpeg (%dx%d)\n", width, height);
    return 0;
}
