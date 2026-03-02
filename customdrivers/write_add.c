#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main() {
    int fd = open("/dev/adddriver", O_WRONLY);
    if (fd < 0) { perror("open"); return 1; }

    int nums[2];
    printf("Enter two numbers: ");
    scanf("%d %d", &nums[0], &nums[1]);

    // write exactly 2 ints as binary
    if (write(fd, nums, sizeof(nums)) != sizeof(nums)) {
        perror("write failed");
        return 1;
    }

    close(fd);
    return 0;
}
