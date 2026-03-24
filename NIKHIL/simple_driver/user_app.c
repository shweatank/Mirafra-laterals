#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define DEVICE_PATH "/dev/reverse_char_dev"
#define BUFFER_SIZE 256

int main(int argc, char *argv[])
{
    int device_file;
    char inputString[BUFFER_SIZE] = {0};
    char outputString[BUFFER_SIZE] = {0};
    ssize_t bytesRead;
    ssize_t bytesWritten;
    
    /* Get input string from command line or use default */
    if (argc > 1) {
        strncpy(inputString, argv[1], BUFFER_SIZE - 1);
    } else {
        strcpy(inputString, "Nikhil");
    }
    
    printf("User Space Application: String Reversal\n");
    printf("=========================================\n");
    printf("Input String: %s\n", inputString);
    printf("Input Length: %zu bytes\n", strlen(inputString));
    
    /* Open device file */
    device_file = open(DEVICE_PATH, O_RDWR);
    if (device_file < 0) {
        perror("Failed to open device. Make sure the driver is loaded and device exists");
        fprintf(stderr, "Device path: %s\n", DEVICE_PATH);
        return errno;
    }
    
    printf("Device file opened successfully (fd: %d)\n\n", device_file);
    
    /* Write string to device (character by character) */
    printf("Writing string to device character by character...\n");
    for (size_t i = 0; i < strlen(inputString); i++) {
        bytesWritten = write(device_file, &inputString[i], 1);
        if (bytesWritten < 0) {
            perror("Failed to write to device");
            close(device_file);
            return errno;
        }
        printf("  Wrote: '%c' (index %zu)\n", inputString[i], i);
    }
    
    printf("\nReading reversed string from device...\n");
    
    /* Seek to beginning for read (simulating a new transaction) */
    lseek(device_file, 0, SEEK_SET);
    
    /* Read reversed string from device */
    bytesRead = read(device_file, outputString, BUFFER_SIZE - 1);
    if (bytesRead < 0) {
        perror("Failed to read from device");
        close(device_file);
        return errno;
    }
    
    outputString[bytesRead] = '\0';
    
    printf("\n=========================================\n");
    printf("Original String:  %s\n", inputString);
    printf("Reversed String:  %s\n", outputString);
    printf("Bytes Reversed:   %zd\n", bytesRead);
    printf("=========================================\n");
    
    /* Verify the reversal */
    char tempBuffer[BUFFER_SIZE] = {0};
    int len = strlen(inputString);
    for (int i = 0; i < len; i++) {
        tempBuffer[i] = inputString[len - 1 - i];
    }
    tempBuffer[len] = '\0';
    
    if (strcmp(outputString, tempBuffer) == 0) {
        printf("Result: SUCCESS - String correctly reversed!\n");
    } else {
        printf("Result: FAILED - String reversal mismatch!\n");
        printf("Expected: %s\n", tempBuffer);
        printf("Got:      %s\n", outputString);
    }
    
    /* Close device file */
    close(device_file);
    
    return 0;
}
