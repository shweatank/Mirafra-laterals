#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/ioctl.h>

#define MAGIC 'a'

struct calc_req{
    int a;
    int b;
    long result;
    int error;
};

#define CALC_IOC_ADD _IOWR(MAGIC, 1, struct calc_req)
#define CALC_IOC_SUB _IOWR(MAGIC, 2, struct calc_req)
#define CALC_IOC_MUL _IOWR(MAGIC, 3, struct calc_req)
#define CALC_IOC_DIV _IOWR(MAGIC, 4, struct calc_req)
#define CALC_IOC_MOD _IOWR(MAGIC, 5, struct calc_req)

int main()
{
    int fd;
    struct calc_req req;

    fd = open("/dev/calci_ioctl", O_RDWR);
    if (fd < 0) {
        perror("open");
        return -1;
    }

    printf("Enter two numbers: ");
    scanf("%d %d", &req.a, &req.b);

    if (ioctl(fd, CALC_IOC_ADD, &req) == -1) {
        perror("ioctl");
        close(fd);
        return -1;
    }

    if (req.error)
        printf("Error occurred!\n");
    else
        printf("Result: %ld\n", req.result);

    close(fd);
    return 0;
}
