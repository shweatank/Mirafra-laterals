#include <stdio.h>
#include <string.h>

#define OP_LEN 16

struct calc_data {
    int a;
    int b;
    char op[OP_LEN];
};

int main()
{
    FILE *fp;
    struct calc_data data;
    int result;

    data.a = 20;
    data.b = 5;
    strcpy(data.op, "DIV");   // Change to ADD, SUB, MUL, DIV

    fp = fopen("/dev/calculator_driver", "r+");
    if (!fp) {
        perror("Open failed");
        return 1;
    }

    fwrite(&data, sizeof(data), 1, fp);
    fread(&result, sizeof(result), 1, fp);

    printf("Result = %d\n", result);

    fclose(fp);
    return 0;
}
