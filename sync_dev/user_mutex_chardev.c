#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define DEVICE "/dev/mutex_chardev"

int main()
{
    int fd;
    char write_buf[] = "Hello from userspace!";
    char read_buf[1024];
    ssize_t bytes;

    printf("[PID %d] Trying to open device...\n", getpid());

    /* open() will block here if another process holds the mutex */
    fd = open(DEVICE, O_RDWR);
    if (fd < 0) {
        perror("open failed");
        return 1;
    }

    printf("[PID %d] Device opened!\n", getpid());

    /* Write to driver */
    bytes = write(fd, write_buf, strlen(write_buf));
    printf("[PID %d] Wrote %ld bytes: %s\n", getpid(), bytes, write_buf);

    /* Sleep 10 seconds — so you can test blocking from another terminal */
    printf("[PID %d] Sleeping 10 seconds, try opening from another terminal!\n", getpid());
    sleep(10);

    /* Read back */
    lseek(fd, 0, SEEK_SET);   /* reset file position to beginning */
    bytes = read(fd, read_buf, sizeof(read_buf));
    read_buf[bytes] = '\0';
    printf("[PID %d] Read %ld bytes: %s\n", getpid(), bytes, read_buf);

    close(fd);
    printf("[PID %d] Device closed, mutex released!\n", getpid());

    return 0;
}
