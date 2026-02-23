#include <stdio.h>
#include <string.h>

int main()
{
    // char *str = "Hello From User\n";
    int arr[] = {100,20};
    int len = sizeof(arr)/sizeof(int);

    // char read_buff[100];
    int sum;

    FILE *fs = fopen("/dev/basic_char", "r+");

    if (fs == NULL) {
        perror("fopen failed");
        return 1;
    }

    /* Write */
    // fwrite(str, sizeof(char), strlen(str), fs);
    fwrite(arr, sizeof(int), len, fs);

    /* Important: flush buffer */
    fflush(fs);

    /* Reset offset */
    fseek(fs, 0, SEEK_SET);

    /* Read */
    int n = fread(&sum, sizeof(int), len, fs);
  
    // read_buff[n] = '\0';

    // printf("String: %s\n", read_buff);
    printf("Sum: %d\n",sum);

    fclose(fs);

    return 0;
}