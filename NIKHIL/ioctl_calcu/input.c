#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define CALC_IOC_MAGIC 'C'

struct calc_req {
    int a;
    int b;
    long result;
    int err;
};
#define CALC_IOC_ADD _IOWR(CALC_IOC_MAGIC, 1, struct calc_req)
int main()
{
    int fd = open("/dev/calc_dev", O_RDWR);
    struct calc_req req;
    req.a = 10;
    req.b = 5;
    ioctl(fd, CALC_IOC_ADD, &req);
    if (req.err)
        printf("Error occurred\n");
    else
        printf("Result = %ld\n", req.result);
    close(fd);
    return 0;
}