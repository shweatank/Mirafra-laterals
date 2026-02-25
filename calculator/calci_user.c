#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

struct calc_data {
    int num1;
    int num2;
    char op;
};

int main()
{
    int fd;
    struct calc_data data;
    int result;

    fd = open("/dev/Calci", O_RDWR);

    data.num1 = 20;
    data.num2 = 10;
    data.op = '*';

    write(fd, &data, sizeof(data));

    read(fd, &result, sizeof(result));

    printf("Result from driver: %d\n", result);

    close(fd);
    return 0;
}
