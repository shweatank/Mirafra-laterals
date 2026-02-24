#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

/* SAME definitions must exist here */

#define CALC_MAGIC 'k'

struct calc_data {
    int a;
    int b;
    int result;
};

#define CALC_IOC_ADD _IOWR(CALC_MAGIC, 1, struct calc_data)
#define CALC_IOC_SUB _IOWR(CALC_MAGIC, 2, struct calc_data)
#define CALC_IOC_MUL _IOWR(CALC_MAGIC, 3, struct calc_data)
#define CALC_IOC_DIV _IOWR(CALC_MAGIC, 4, struct calc_data)

#define DEVICE "/dev/calcioctdev"

int main()
{
    int fd, choice;
    struct calc_data data;

    fd = open(DEVICE, O_RDWR);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    printf("Enter two numbers: ");
    scanf("%d %d", &data.a, &data.b);

    printf("\nChoose operation:\n");
    printf("1.Add  2.Sub  3.Mul  4.Div\n");
    scanf("%d", &choice);

    switch(choice) {
        case 1:
            ioctl(fd, CALC_IOC_ADD, &data);
            break;
        case 2:
            ioctl(fd, CALC_IOC_SUB, &data);
            break;
        case 3:
            ioctl(fd, CALC_IOC_MUL, &data);
            break;
        case 4:
            ioctl(fd, CALC_IOC_DIV, &data);
            break;
        default:
            printf("Invalid choice\n");
            return 0;
    }

    printf("Result = %d\n", data.result);

    close(fd);
    return 0;
}
