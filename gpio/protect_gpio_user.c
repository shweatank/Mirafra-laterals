#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{
    int fd = open("/dev/simple_gpio", O_WRONLY);
    if (fd < 0) {
        perror("open");
        return -1;
    }

    write(fd, "1", 1);
    sleep(1);
    write(fd, "0", 1);

    close(fd);
    return 0;
}
