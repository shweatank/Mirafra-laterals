#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{
    int fd;
    char buf[50];

    fd = open("/dev/kbdcalc", O_WRONLY);

    printf("Enter two numbers: ");
    fgets(buf, sizeof(buf), stdin);

    write(fd, buf, sizeof(buf));

    close(fd);
    return 0;
}
