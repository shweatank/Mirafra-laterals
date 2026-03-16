#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

#define PORT 9000

int main()
{
    int sock;
    struct sockaddr_in server;
    char buffer[256];

    sock = socket(AF_INET, SOCK_STREAM, 0);

    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = inet_addr("10.16.201.122");

    connect(sock,(struct sockaddr*)&server,sizeof(server));
    send(sock, "RECEIVER", 8, 0);

    while(1)
    {
        int n = recv(sock, buffer, sizeof(buffer), 0);

        if(n <= 0)
            break;

        buffer[n] = '\0';

        printf("Received from Pi1: %s", buffer);
    }

    close(sock);
}
