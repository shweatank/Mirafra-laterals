#include <stdio.h>
#include <stdlib.h>

int main(){
    int arr[2] = {10, 0};
    int result;
    char operation[16] = "DIV";

    FILE *fp = fopen("/dev/function_pointer_in_driver", "r+");
    if(fp == NULL) {
        perror("file open failed");
        return 1;
    }

    fwrite(arr, sizeof(int), 2, fp);
    fwrite(operation, sizeof(operation), 1, fp);

    fflush(fp);   
    rewind(fp); 

    fread(&result, sizeof(result), 1, fp);

    printf("Result is %d\n", result);
    fclose(fp);
}
