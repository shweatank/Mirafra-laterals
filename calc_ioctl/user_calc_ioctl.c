#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define CALC_IOC_MAGIC  'B'

struct calc_req {
    int  a;
    int  b;
    long result;
    int  err;
};

#define CALC_IOC_ADD  _IOWR(CALC_IOC_MAGIC, 1, struct calc_req)
#define CALC_IOC_SUB  _IOWR(CALC_IOC_MAGIC, 2, struct calc_req)
#define CALC_IOC_MUL  _IOWR(CALC_IOC_MAGIC, 3, struct calc_req)
#define CALC_IOC_DIV  _IOWR(CALC_IOC_MAGIC, 4, struct calc_req)
#define CALC_IOC_MOD  _IOWR(CALC_IOC_MAGIC, 5, struct calc_req)

static void calc(int fd, unsigned long cmd, const char *op, int a, int b)
{
    struct calc_req req;
    req.a = a;
    req.b = b;

    if (ioctl(fd, cmd, &req) < 0) {
        perror("ioctl");
        return;
    }

    if (req.err) {
        printf("Div by 0 not valid\n");
    } else {
        printf("  %d %s %d  is  %ld\n", a, op, b, req.result);
    }
}

int main(void)
{
    int fd;

    fd = open("/dev/calc_ioctl", O_RDWR);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    int a = 20, b = 6;

    calc(fd, CALC_IOC_ADD, "ADD",   a, b);
    calc(fd, CALC_IOC_SUB, "SUB",   a, b);
    calc(fd, CALC_IOC_MUL, "MUL",   a, b);
    calc(fd, CALC_IOC_DIV, "DIV",   a, b);
    calc(fd, CALC_IOC_MOD, "MOD",   a, b);
    calc(fd, CALC_IOC_DIV, "DIV",   a, 0);


    close(fd);
    return 0;
}
