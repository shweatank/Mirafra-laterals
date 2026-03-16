#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <poll.h>
#include <stdlib.h>

#define PORT 9000
#define SERVER_IP "192.168.0.195"

int main()
{
    int sock, timerfd;
    struct sockaddr_in server;

    char msg[128];
    char tick[32];
    char last[128] = "";

    /* Create socket */
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0)
    {
        perror("socket");
        return -1;
    }

    /* Server details */
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = inet_addr(SERVER_IP);

    if(connect(sock,(struct sockaddr*)&server,sizeof(server)) < 0)
    {
        perror("connect");
        return -1;
    }

    printf("Connected to server\n");

    /* Register as sender */
    send(sock,"SENDER\n",7,0);

    /* Open timer device */
    timerfd = open("/dev/timerdev",O_RDONLY);
    if(timerfd < 0)
    {
        perror("timer open");
        return -1;
    }

    struct pollfd pfd;
    pfd.fd = timerfd;
    pfd.events = POLLIN;

    while(1)
{
    poll(&pfd,1,-1);

    read(timerfd,tick,sizeof(tick));

    int lcd = open("/dev/ili9225_rani",O_RDONLY);

    if(lcd < 0)
    {
        perror("lcd open");
        continue;
    }

    int n = read(lcd,msg,sizeof(msg)-1);

    close(lcd);

    if(n <= 0)
        continue;

    msg[n] = '\0';

    msg[strcspn(msg,"\n")] = 0;

    send(sock,msg,strlen(msg),0);

    printf("Sent to server: %s\n",msg);
}

    close(timerfd);
    close(sock);

    return 0;
}
