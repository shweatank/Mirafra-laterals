#include<stdio.h>
#include<signal.h>
#include<sys/types.h>
#include<unistd.h>
void sig_handler(int signum)
{
    printf("hello",signum);
}
int main()
{
    signal(SIGINT, sig_handler);
    while (1)
    {
        sleep(1);
    }

}