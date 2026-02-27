#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{
    int fd = open("/dev/uart2", O_RDWR);

    char tx='K';
    char rx;

    write(fd, &tx, 1);

    read(fd, &rx, 1);

    printf("Received = %c\n", rx);

    close(fd);
}
