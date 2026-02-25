#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define MAGIC 'a'
#define IOCTL_SET_VALUE _IOWR(MAGIC, 1, int)

int main()
{
    int fd;
    int value = 50;

    fd = open("/dev/basic_ioctl", O_RDWR);

    if (fd < 0) {
        perror("open");
        return -1;
    }

    printf("Before ioctl: %d\n", value);

    ioctl(fd, IOCTL_SET_VALUE, &value);

    printf("After ioctl (returned): %d\n", value);

    close(fd);
    return 0;
}
