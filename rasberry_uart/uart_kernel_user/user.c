#include <stdio.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>

#define CALC_IOC_MAGIC 'B'

struct uart_req {
    char tx_buf[100];
    char rx_buf[100];
    int  len;
};

#define UART_IOC_TXRX _IOWR(CALC_IOC_MAGIC, 1, struct uart_req)

int main()
{
    int fd;
    struct uart_req req;

    fd = open("/dev/calc_ioctl_class", O_RDWR);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    strcpy(req.tx_buf, "HELLO");
    req.len = strlen(req.tx_buf);

    if (ioctl(fd, UART_IOC_TXRX, &req) < 0) {
        perror("ioctl");
        return 1;
    }

    req.rx_buf[req.len] = '\0';

    printf("Sent: %s\n", req.tx_buf);
    printf("Received: %s\n", req.rx_buf);

    close(fd);
    return 0;
}