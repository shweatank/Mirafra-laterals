#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define PORT 9000

int main()
{
    int sock,lcd;
    struct sockaddr_in server;
    char buffer[128];

    sock = socket(AF_INET,SOCK_STREAM,0);

    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = inet_addr("10.16.201.122");

    connect(sock,(struct sockaddr*)&server,sizeof(server));

    send(sock,"RECEIVER",8,0);

    lcd = open("/dev/ili9225_rani",O_WRONLY);

	if(lcd < 0)
	{
    	perror("LCD open failed");
    	return -1;
	}

    while(1)
    {
        int n = recv(sock,buffer,sizeof(buffer)-1,0);

	if(n<=0)
    	break;

	buffer[n] = '\0';

	printf("Displayed: %s",buffer);

	int w = write(lcd,buffer,strlen(buffer));

if(w < 0)
    perror("LCD write failed");
    }
}
