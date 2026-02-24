#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>

#define MAGIC_NUM 'B'
#define SET_DATA _IOW(MAGIC_NUM, 1, struct input_output)
#define GET_DATA _IOR(MAGIC_NUM, 2, struct input_output)

struct input_output{
    int a;
    int b;
    int result;
};

int main(){
    struct input_output io;
    io.a=10;
    io.b=10;
    io.result = 0;
    
    int fd = open("/dev/INTR_DEVICE",O_RDWR);


    ioctl(fd, SET_DATA, &io);

    printf("Press A for add or S for sub...\n");
    sleep(2);

    ioctl(fd, GET_DATA, &io);

    printf("Result = %d\n", io.result);
    return 0;
}