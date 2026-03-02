#include <stdio.h>
#include <string.h>

int main()
{
    FILE *fp2, *fp1;
    char filename[100];
    char buffer[200];

    // Open file2 to get file1 name
    fp2 = fopen("file2.txt", "r");
    if (fp2 == NULL) {
        printf("Error opening file2\n");
        return 1;
    }

    fgets(filename, sizeof(filename), fp2);
    fclose(fp2);

    //Remove newline character, or else it will take as "filename\n", so print file not present/error opening file
    filename[strcspn(filename, "\n")] = 0;

    //Open file1 using name from file2
    fp1 = fopen(filename, "r");
    if (fp1 == NULL) {
        printf("Error opening file1\n");
        return 1;
    }

    while (fgets(buffer, sizeof(buffer), fp1)) {
        printf("%s", buffer);
    }

    fclose(fp1);

    return 0;
}
