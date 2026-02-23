#include <stdio.h>
#include <stdlib.h>

int main(){
    int arr[2] = {10, 20};
    int result;

    FILE *fp = fopen("/dev/function_pointer_in_driver", "r+");
    if(fp == NULL) {
        perror("file open failed");
        return 1;
    }

    fwrite(arr, sizeof(int), 2, fp);
    
    fread(&result, sizeof(result), 1, fp);

    printf("Sum is %d\n", result);
    fclose(fp);
}