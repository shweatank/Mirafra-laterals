#include<stdio.h>
#include<unistd.h>
#include <sys/shm.h>
#include<string.h>


int main()
{
int key= shm_ope("shmfile",65);
int smemory= shmget(k,1024,0666 | IPC_CREAT);
char *s=(char *)shmat(shmid,(void *)0,0);
printf("write Data");
fgets(str,1024,stdin);
    return 0;
}