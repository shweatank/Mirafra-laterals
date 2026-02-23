#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define CALC_IOC_MAGIC 'B'

struct cal_req {
    int a;
    int b;
    long result;
    int err;
};

#define CALC_IOC_ADD _IOWR(CALC_IOC_MAGIC,1,struct cal_req)
#define CALC_IOC_SUB _IOWR(CALC_IOC_MAGIC,2,struct cal_req)
#define CALC_IOC_MUL _IOWR(CALC_IOC_MAGIC,3,struct cal_req)
#define CALC_IOC_DIV _IOWR(CALC_IOC_MAGIC,4,struct cal_req)
#define CALC_IOC_MOD _IOWR(CALC_IOC_MAGIC,5,struct cal_req)

int main()
{
    int fd;
    struct cal_req cal;

    fd = open("/dev/ioctl_calu", O_RDWR);
    if (fd < 0) {
        perror("open");
        return -1;
    }

    cal.a = 20;
    cal.b = 30;

    printf("User: sending a=%d b=%d\n", cal.a, cal.b);

    if (ioctl(fd, CALC_IOC_ADD, &cal) < 0) {
        perror("ioctl");
        close(fd);
        return -1;
    }

    if (cal.err == 0)
        printf("Result = %ld\n", cal.result);
    else
        printf("Kernel returned error = %d\n", cal.err);

    close(fd);
    return 0;
}