#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define DEVICE "/dev/char_driver"
#define BUF_SIZE 256

int main()
{
    int fd;
    char write_buf[] = "Hello from User Space!";
    char read_buf[BUF_SIZE];

    /* Open device */
    fd = open("/dev/char_driver", O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return 1;
    }

    printf("Device opened successfully\n");

    /* Write to device */
    write(fd, write_buf, strlen(write_buf));
    printf("Written to device: %s\n", write_buf);

    /* Move file pointer to beginning */
    lseek(fd, 0, SEEK_SET);

    /* Read from device */
    read(fd, read_buf, BUF_SIZE);
    printf("Read from device: %s\n", read_buf);

    /* Close device */
    close(fd);

    return 0;
}
