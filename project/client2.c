// client_rpi2.c
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
    write(lcd_fd, msg, strlen(msg));

}

int main()
{
    int sock;
    struct sockaddr_in serv_addr;
    char buffer[1024];

    sock = socket(AF_INET, SOCK_STREAM, 0);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr);

    connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
   lcd_fd = open("/dev/ili9225_char",O_WRONLY); 
    while(1)
    {
        int n = read(sock, buffer, sizeof(buffer)-1);

        if(n > 0)
        {
	   buffer[n] = '\0';
           printf("Received from server: %s\n", buffer);
            lcd_display(buffer);
        }
    }
}
