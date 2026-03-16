#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#define PORT 9000

int main()
{
int server_fd, sender=-1, receiver=-1, client;
struct sockaddr_in addr;
char buffer[256];
server_fd = socket(AF_INET, SOCK_STREAM, 0);

addr.sin_family = AF_INET;
addr.sin_port = htons(PORT);
addr.sin_addr.s_addr = INADDR_ANY;

bind(server_fd,(struct sockaddr*)&addr,sizeof(addr));
listen(server_fd,5);

printf("Waiting for clients\n");

while(sender==-1 || receiver==-1)
{
    client = accept(server_fd,NULL,NULL);

    int n = recv(client,buffer,sizeof(buffer)-1,0);
    buffer[n]='\0';

    printf("Client says: %s\n",buffer);

    if(strncmp(buffer,"SENDER",6)==0)
    {
        sender = client;
        printf("Pi1 registered as SENDER\n");
    }
    else if(strncmp(buffer,"RECEIVER",8)==0)
    {
        receiver = client;
        printf("Pi2 registered as RECEIVER\n");
    }
}

printf("Forwarding data\n");

while(1)
{
    int n = recv(sender,buffer,sizeof(buffer),0);

    if(n<=0) break;

    buffer[n]='\0';

    printf("Received from Pi1: %s\n",buffer);

    send(receiver,buffer,n,0);
}

}

