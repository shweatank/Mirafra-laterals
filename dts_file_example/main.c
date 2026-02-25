#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

struct student {
    char name[50];
    int year;
    char branch[50];
};

int main() {

    int fd1, fd2;
    char filepath[200];
    char buffer[200];
    struct student s;

    /* Open file1.txt (contains path of file2) */
    fd1 = open("/home/mirafra/LDD_GITPUSSH/Mirafra-laterals/dts_file_example/file1.txt", O_RDONLY);
    if (fd1 < 0) {
        perror("Error opening file1");
        return 1;
    }

    /* Read file2 path */
    int bytes = read(fd1, filepath, sizeof(filepath) - 1);
    if (bytes <= 0) {
        perror("Error reading file1");
        close(fd1);
        return 1;
    }

    filepath[bytes] = '\0';  // null terminate
    close(fd1);

    /* Open file2 */
    fd2 = open(filepath, O_RDONLY);
    if (fd2 < 0) {
        perror("Error opening file2");
        return 1;
    }

    /* Read file2 data */
    bytes = read(fd2, buffer, sizeof(buffer) - 1);
    if (bytes <= 0) {
        perror("Error reading file2");
        close(fd2);
        return 1;
    }

    buffer[bytes] = '\0';
    close(fd2);

    /* Parse data into struct */
    sscanf(buffer, "%s %d %s", s.name, &s.year, s.branch);

    /* Print */
    printf("Name   : %s\n", s.name);
    printf("Year   : %d\n", s.year);
    printf("Branch : %s\n", s.branch);

    return 0;
}