#include <stdio.h>
#include <string.h>


#define sum 1
#define sub 2
#define mul 3
#define div 4

struct data{
    int ops;
    int a;
    int b;
    int result;
};

int main()
{
    // char *str = "Hello From User\n";
    int arr[] = {100,20};
    int len = sizeof(arr)/sizeof(int);

    // char read_buff[100];
    // int sum;
    struct data d;

    d.ops = sum;
    d.a = 10;
    d.b = 10;

    FILE *fs = fopen("/dev/basic_char", "r+");

    if (fs == NULL) {
        perror("fopen failed");
        return 1;
    }

    /* Write */
    // fwrite(str, sizeof(char), strlen(str), fs);
    fwrite(&d, sizeof(d), 1, fs);

    /* Important: flush buffer */
    fflush(fs);

    /* Reset offset */
    fseek(fs, 0, SEEK_SET);

    /* Read */
    int n = fread(&d, sizeof(d), 1, fs);
  
    // read_buff[n] = '\0';

    // printf("String: %s\n", read_buff);
    printf("Ret: %d\n",d.result);

    fclose(fs);

    return 0;
}