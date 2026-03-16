#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/timerfd.h>
#include <stdint.h>

#define PORT 9000

int main()
{
int sock,lcd,timerfd;
struct sockaddr_in server;
char msg[128];
uint64_t expirations;

sock = socket(AF_INET,SOCK_STREAM,0);

server.sin_family = AF_INET;
server.sin_port = htons(PORT);
server.sin_addr.s_addr = inet_addr("10.16.201.122");

connect(sock,(struct sockaddr*)&server,sizeof(server));

send(sock,"SENDER\n",7,0);

lcd = open("/dev/ili9225_rani",O_RDONLY);

if(lcd < 0)
{
    perror("LCD open failed");
    return -1;
}

timerfd = timerfd_create(CLOCK_MONOTONIC,0);

struct itimerspec timer;
timer.it_value.tv_sec = 5;
timer.it_value.tv_nsec = 0;
timer.it_interval.tv_sec = 5;
timer.it_interval.tv_nsec = 0;

timerfd_settime(timerfd,0,&timer,NULL);

while(1)
{
    read(timerfd,&expirations,sizeof(expirations)); // wait for timer

    int n = read(lcd,msg,sizeof(msg));

    if(n > 0)
    {
        msg[n] = '\0';

        send(sock,msg,strlen(msg),0);

        printf("Sent LCD text: %s\n",msg);
    }
}

}

