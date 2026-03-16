// client_rpi1.c
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <arpa/inet.h>

#define PORT 8080
#define SERVER_IP "192.168.0.56"
int lcd_fd;
void lcd_display(char *msg)
{
    write(lcd_fd,msg,strlen(msg));
}

int main()
{
    int sock, fd;
    struct sockaddr_in serv_addr;
    char buffer[100];

    sock = socket(AF_INET, SOCK_STREAM, 0);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr);

    connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    fd = open("/dev/timer_device", O_RDONLY);
    lcd_fd = open("/dev/ili9225_char", O_WRONLY);
    if(lcd_fd < 0)
    {
        perror("LCD open failed");
        return -1;
    }
    while(1)
    {
        int n = read(fd, buffer, sizeof(buffer));

        if(n > 0)
        {
            lcd_display(buffer);
            send(sock, buffer, n, 0);
        }

        sleep(1);
    }
}
