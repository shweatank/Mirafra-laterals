// server_receive.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080

int main()
{
    int server_fd, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    char buffer[1024];

    /* Create socket */
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd < 0)
    {
        perror("Socket failed");
        exit(1);
    }

    /* Server configuration */
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    /* Bind socket */
    if(bind(server_fd,(struct sockaddr *)&server_addr,sizeof(server_addr)) < 0)
    {
        perror("Bind failed");
        exit(1);
    }

    /* Listen for client */
    listen(server_fd,1);

    printf("Waiting for RPI1 connection...\n");

    /* Accept connection */
    client_sock = accept(server_fd,(struct sockaddr *)&client_addr,&addr_len);

    printf("RPI1 connected\n");

    /* Receive loop */
    while(1)
    {
        int n = recv(client_sock,buffer,sizeof(buffer)-1,0);

        if(n > 0)
        {
            buffer[n] = '\0';
            printf("Received from RPI1: %s\n",buffer);
        }
        else if(n == 0)
        {
            printf("Client disconnected\n");
            break;
        }
        else
        {
            perror("Receive error");
            break;
        }
    }

    close(client_sock);
    close(server_fd);

    return 0;
}
