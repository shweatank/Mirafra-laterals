#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <poll.h>

#define PORT 9000

int main()
{
int sock,timerfd,lcd;
struct sockaddr_in server;
char msg[128];

sock = socket(AF_INET,SOCK_STREAM,0);

server.sin_family = AF_INET;
server.sin_port = htons(PORT);
server.sin_addr.s_addr = inet_addr("10.16.201.122");

connect(sock,(struct sockaddr*)&server,sizeof(server));

send(sock,"SENDER\n",7,0);
sleep(1);

timerfd = open("/dev/timerdev",O_RDONLY);
lcd = open("/dev/ili9225_rani",O_RDONLY);

struct pollfd pfd;
pfd.fd = timerfd;
pfd.events = POLLIN;

while(1)
{
    poll(&pfd,1,-1);   // wait timer interrupt

    int n = read(lcd,msg,sizeof(msg));

    if(n <= 0)
        continue;

    msg[n] = '\0';

    if(strlen(msg) == 0)
        continue;

	printf("Buffer read from LCD: %s\n",msg);
	send(sock,msg,strlen(msg),0);

    printf("Sent LCD text: %s\n",msg);
}

}

