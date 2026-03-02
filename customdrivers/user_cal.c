#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define DEVICE "/dev/calcdev"

int main()
{
    int fd;
    char input[100];
    char output[100];

    fd = open(DEVICE, O_RDWR);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    printf("Enter: num1 num2 operation\n");
    printf("Example: 5 3 add\n");

    fgets(input, sizeof(input), stdin);

    write(fd, input, strlen(input));
    read(fd, output, sizeof(output));

    printf("Result from driver: %s\n", output);

    close(fd);
    return 0;
}
