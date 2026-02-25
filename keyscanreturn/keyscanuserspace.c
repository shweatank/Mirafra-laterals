#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{
    int fd;
    int numbers[2];
    int result;

    fd = open("/dev/irq_calci", O_RDWR);
    if (fd < 0) {
        perror("open");
        return -1;
    }

    printf("Enter two numbers: ");
    scanf("%d %d", &numbers[0], &numbers[1]);

    write(fd, numbers, sizeof(numbers));

    printf("Press key:\n");
    printf("A -> Add\n");
    printf("S -> Sub\n");
    printf("M -> Mul\n");
    printf("D -> Div\n");

    getchar();

    read(fd, &result, sizeof(result));

    printf("Result from Kernel: %d\n", result);

    close(fd);
    return 0;
}
