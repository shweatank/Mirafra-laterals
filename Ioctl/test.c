#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/ioctl.h>

#define IOCTL_MAGIC	'A'
#define IOCTL_SET_VALUE	_IOWR(IOCTL_MAGIC, 1, int)

int main()
{
	int fd;
	int val = 20;

	fd = open("/dev/ioctl", O_RDWR);
	if(fd < 0)
	{
		perror("open");
		return 1;
	}

	printf("User: sending %d to kernel\n", val);

	ioctl(fd, IOCTL_SET_VALUE, &val);

	printf("User: received %d from kernel\n", val);

	close(fd);

	return 0;
}
