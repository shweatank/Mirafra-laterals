// client_rpi2.c
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <arpa/inet.h>

#define PORT 8080
#define SERVER_IP "192.168.0.87"

int lcd_fd;
void lcd_clear()
{
    char clear_buf[32] = "                                "; // 32 spaces
    write(lcd_fd, clear_buf, sizeof(clear_buf));
}
void lcd_display(char *msg)
{
    write(lcd_fd,msg,strlen(msg));
}

int main()
{
    int sock;
    struct sockaddr_in serv_addr;
    char buffer[1024];

    sock = socket(AF_INET,SOCK_STREAM,0);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    inet_pton(AF_INET,SERVER_IP,&serv_addr.sin_addr);

    connect(sock,(struct sockaddr *)&serv_addr,sizeof(serv_addr));
    lcd_fd = open("/dev/ili9225_isshika",O_WRONLY);
    int count=0;    
    while(1)
    {
       
        int n = recv(sock,buffer,sizeof(buffer),0);

        if(n > 0)
        {
           count++;
            buffer[n] = '\0';

            printf("Received: %s\n",buffer);

            lcd_display(buffer);
        }
     if (count == 10)
            {
                printf("10 interrupts reached. Clearing LCD and exiting...\n");

                lcd_clear();

                break;
            }

    }
}
