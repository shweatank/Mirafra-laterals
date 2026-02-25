#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{
    int fd;
    char buf[50];

    fd = open("/dev/kbdcalc", O_RDWR);

    printf("Enter two numbers: ");
    fgets(buf, sizeof(buf), stdin);

    write(fd, buf, sizeof(buf));

    printf("Press a/s/m/d key to compute...\n");
    getchar();   // wait for key press

    read(fd, buf, sizeof(buf));

    printf("Driver returned: %s\n", buf);

    close(fd);
    return 0;
}
