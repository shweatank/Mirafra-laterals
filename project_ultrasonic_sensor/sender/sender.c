#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>
#include <poll.h>

void uart_init(int fd)
{
    struct termios tty;

    tcgetattr(fd, &tty);

    cfsetospeed(&tty, B9600);
    cfsetispeed(&tty, B9600);

    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;
    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;

    tty.c_lflag = 0;
    tty.c_oflag = 0;
    tty.c_iflag = 0;

    tty.c_cc[VMIN]  = 1;
    tty.c_cc[VTIME] = 1;

    tcsetattr(fd, TCSANOW, &tty);
}

int main()
{
    int fd_dev, fd_uart;
    char buf[100];
    char prev_buf[100] = {0};

    struct pollfd pfd;

    fd_dev = open("/dev/us_dev", O_RDONLY);
    if (fd_dev < 0)
    {
        perror("Device open failed");
        return -1;
    }

    fd_uart = open("/dev/serial0", O_RDWR | O_NOCTTY);
    if (fd_uart < 0)
    {
        perror("UART open failed");
        return -1;
    }

    uart_init(fd_uart);

    pfd.fd = fd_dev;
    pfd.events = POLLIN;

    printf("Waiting for interrupt-driven data...\n");

    while (1)
    {
        int ret = poll(&pfd, 1, -1);  // BLOCK until IRQ

        if (ret > 0)
        {
            memset(buf, 0, sizeof(buf));

            int n = read(fd_dev, buf, sizeof(buf) - 1);
            if (n <= 0)
                continue;

            buf[n] = '\0';
            buf[strcspn(buf, "\n")] = 0;

            // 🔥 Skip duplicates
            if (strcmp(buf, prev_buf) == 0)
                continue;

            strcpy(prev_buf, buf);

            char tx_buf[100];
            snprintf(tx_buf, sizeof(tx_buf), "%s \n", buf);

            write(fd_uart, tx_buf, strlen(tx_buf));

            printf("Sent: %s", tx_buf);
	   sleep(3);
        }
    }

    close(fd_dev);
    close(fd_uart);

    return 0;
}
