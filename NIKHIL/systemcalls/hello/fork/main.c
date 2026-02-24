#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

    int num=10;
                int a,b;

int main()
{
	pid_t child_pid;
	child_pid = fork();
	if(child_pid == 0){

		printf("Self ID %d\n",(int)getpid());
		printf("parent ID %d\n",(int )getppid());
        scanf("enter the data %d %d\n",&a,&b);
        //sleep(3);
    }
    wait(NULL);
    execlp("ls","ls",NULL);
    printf("result = %d",a+b);


	
	return 0;
}
