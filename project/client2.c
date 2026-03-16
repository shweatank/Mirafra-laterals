#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include<stdlib.h>

#define PORT 9000

int main()
{
    int sock, fd;
    struct sockaddr_in server;
    char buffer[100];

    sock = socket(AF_INET, SOCK_STREAM, 0);

    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = inet_addr("10.16.201.71"); // Laptop IP

    connect(sock,(struct sockaddr*)&server,sizeof(server));
    send(sock, "SENDER", 6, 0);

    printf("Connected to server\n");

    int last = -1;

while(1)
{
    fd = open("/dev/timerdev", O_RDONLY);

    int n = read(fd, buffer, sizeof(buffer));

    int val = atoi(buffer);

    if(val != last)
    {
        send(sock, buffer, n, 0);
        last = val;
    }

    close(fd);

    sleep(1);
}

    close(sock);
}
