#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{
    int fd;
    char ch;

    fd = open("/dev/my_uart", O_RDWR);
    if (fd < 0) {
        perror("open");
        return -1;
    }

    write(fd, "A", 1);

    read(fd, &ch, 1);

    printf("Received: %c\n", ch);

    close(fd);
    return 0;
}
