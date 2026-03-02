#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{
    int fd;
    char ch;

    fd = open("/dev/uartdriver", O_RDWR);
    if (fd < 0) {
        perror("open");
        return -1;
    }

    write(fd, "R", 1);

    read(fd, &ch, 1);

    printf("Received: %c\n", ch);

    close(fd);
    return 0;
}
