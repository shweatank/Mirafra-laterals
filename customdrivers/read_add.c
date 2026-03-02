#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
    int fd = open("/dev/adddriver", O_RDONLY);
    if (fd < 0) { perror("open"); return 1; }

    char buf[50];
    ssize_t n = read(fd, buf, sizeof(buf)-1);
    if (n < 0) { perror("read"); close(fd); return 1; }

    buf[n] = '\0';   // null-terminate
    printf("%s", buf);

    close(fd);
    return 0;
}
