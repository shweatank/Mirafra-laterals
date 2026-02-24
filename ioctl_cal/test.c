#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<sys/ioctl.h>

#define CALC_IOC_MAGIC	'B'

struct calc_req
{
        int num1;
        int num2;
        long res;
        int err;
};

#define CALC_IOC_ADD    _IOWR(CALC_IOC_MAGIC, 1, struct calc_req)
#define CALC_IOC_SUB    _IOWR(CALC_IOC_MAGIC, 2, struct calc_req)
#define CALC_IOC_MUL    _IOWR(CALC_IOC_MAGIC, 3, struct calc_req)
#define CALC_IOC_DIV    _IOWR(CALC_IOC_MAGIC, 4, struct calc_req)
#define CALC_IOC_MOD    _IOWR(CALC_IOC_MAGIC, 5, struct calc_req)

int main()
{
	int fd;
	struct calc_req cal = {.num1 = 10, .num2 = 5};
	
	fd = open("/dev/calc", O_RDWR);
	if(fd < 0)
	{
		perror("open");
		return 1;
	}

	printf("\nAddition\n");
	printf("User: sending %d, %d to kernel\n", cal.num1, cal.num2);

	ioctl(fd, CALC_IOC_ADD, &cal);

	printf("User: received %ld from kernel\n", cal.res);

	printf("\nSubtraction\n");
	printf("User: sending %d, %d to kernel\n", cal.num1, cal.num2);

        ioctl(fd, CALC_IOC_SUB, &cal);

        printf("User: received %ld from kernel\n", cal.res);

	printf("\nMultiplication\n");
	printf("User: sending %d, %d to kernel\n", cal.num1, cal.num2);

        ioctl(fd, CALC_IOC_MUL, &cal);

        printf("User: received %ld from kernel\n", cal.res);

	printf("\nDivision\n");
	printf("User: sending %d, %d to kernel\n", cal.num1, cal.num2);

        ioctl(fd, CALC_IOC_DIV, &cal);

        printf("User: received %ld from kernel\n", cal.res);

	printf("\nModulous\n");
	printf("User: sending %d, %d to kernel\n", cal.num1, cal.num2);

        ioctl(fd, CALC_IOC_MOD, &cal);

        printf("User: received %ld from kernel\n", cal.res);


	close(fd);

	return 0;
}
