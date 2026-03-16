#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <arpa/inet.h>

#define PORT 8080
#define SERVER_IP "192.168.0.87"

int lcd_fd;

void lcd_display(char *msg)
{
    write(lcd_fd, msg, strlen(msg));
}

void lcd_clear()
{
    char clear_buf[64];
    memset(clear_buf, ' ', sizeof(clear_buf));   // fill spaces
    write(lcd_fd, clear_buf, sizeof(clear_buf));
}

int main()
{
    int sock, fd;
    struct sockaddr_in serv_addr;
    char buffer[100];
    char user_msg[100];
    int count = 0;

    printf("Enter message: ");
    fgets(user_msg, sizeof(user_msg), stdin);
    user_msg[strcspn(user_msg, "\n")] = 0;

    sock = socket(AF_INET, SOCK_STREAM, 0);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr);

    connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    printf("Connected to server\n");
    fd = open("/dev/timer_isshika", O_RDONLY);
    lcd_fd = open("/dev/ili9225_iss", O_WRONLY);

    while (1)
    {
        int n = read(fd, buffer, sizeof(buffer));

        if (n > 0)
        {
            count++;

            printf("Timer interrupt %d\n", count);

            lcd_display(user_msg);

            send(sock, user_msg, strlen(user_msg), 0);

            memset(buffer, 0, sizeof(buffer));   // clear buffer

            if (count == 10)
            {
                printf("10 interrupts reached. Clearing LCD and exiting...\n");

                lcd_clear();

                break;
            }
        }
    }

    close(fd);
    close(lcd_fd);
    close(sock);

    return 0;
}
