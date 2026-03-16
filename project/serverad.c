#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>

#define PORT 8080

int main()
{
    int server_fd, client1, client2;
    struct sockaddr_in addr;
    char buffer[1024];

    fd_set readfds;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    bind(server_fd,(struct sockaddr *)&addr,sizeof(addr));
    listen(server_fd,2);

    printf("Waiting for client1...\n");
    client1 = accept(server_fd,NULL,NULL);

    printf("Waiting for client2...\n");
    client2 = accept(server_fd,NULL,NULL);

    printf("Both clients connected\n");

    while(1)
    {
        FD_ZERO(&readfds);

        FD_SET(client1,&readfds);
        FD_SET(client2,&readfds);

        int maxfd = (client1 > client2 ? client1 : client2) + 1;

        select(maxfd,&readfds,NULL,NULL,NULL);

        if(FD_ISSET(client1,&readfds))
        {
            int n = recv(client1,buffer,sizeof(buffer)-1,0);

            if(n > 0)
            {
                buffer[n] = '\0';

                printf("From client1: %s\n",buffer);

                send(client2,buffer,n,0);
            }
        }

        if(FD_ISSET(client2,&readfds))
        {
            int n = recv(client2,buffer,sizeof(buffer)-1,0);

            if(n > 0)
            {
                buffer[n] = '\0';

                printf("From client2: %s\n",buffer);

                send(client1,buffer,n,0);
            }
        }
    }
}
