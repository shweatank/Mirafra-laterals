#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>

#define CALC_IOC_MAGIC 'B'

struct calc_req {
    char name[50];
};

#define CALC_IOC_SEND_NAME _IOWR(CALC_IOC_MAGIC, 1, struct calc_req)

int main()
{
    int fd;
    struct calc_req req;

    fd = open("/dev/calc_ioctl_class", O_RDWR);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    strcpy(req.name, "Nikhil");

    if (ioctl(fd, CALC_IOC_SEND_NAME, &req) < 0) {
        perror("ioctl");
        close(fd);
        return 1;
    }

    printf("User space received : %s\n", req.name);

    close(fd);
    return 0;
}
