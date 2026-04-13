#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/epoll.h>
#include <string.h>

int main()
{
    int fd = open("/dev/epoll_demo", O_RDONLY);
    char buf[64];

    int epfd = epoll_create1(0);
    struct epoll_event ev = {
        .events = EPOLLIN,
        .data.fd = fd
    };

    epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);

    printf("Waiting for data...\n");
    epoll_wait(epfd, &ev, 1, -1);

    read(fd, buf, sizeof(buf));
    printf("Read: %s\n", buf);

    close(fd);
    close(epfd);
    return 0;
}
