#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

#define OLED_ADDR 0x3C

int fd;

/* Send command */
void cmd(unsigned char c)
{
    unsigned char buf[2] = {0x00, c};
    write(fd, buf, 2);
}

/* Send data */
void data(unsigned char d)
{
    unsigned char buf[2] = {0x40, d};
    write(fd, buf, 2);
}

/* Init OLED */
void oled_init()
{
    cmd(0xAE); // OFF
    cmd(0x20); cmd(0x00);
    cmd(0xB0);
    cmd(0xC8);
    cmd(0x00);
    cmd(0x10);
    cmd(0x40);
    cmd(0x81); cmd(0xFF);
    cmd(0xA1);
    cmd(0xA6);
    cmd(0xA8); cmd(0x3F);
    cmd(0xA4);
    cmd(0xD3); cmd(0x00);
    cmd(0xD5); cmd(0xF0);
    cmd(0xD9); cmd(0x22);
    cmd(0xDA); cmd(0x12);
    cmd(0xDB); cmd(0x20);
    cmd(0x8D); cmd(0x14);
    cmd(0xAF); // ON
}

/* Fill screen */
void oled_fill()
{
    for(int i=0; i<1024; i++)
        data(0xFF);   // all pixels ON
}

int main()
{
    fd = open("/dev/i2c-1", O_RDWR);
    ioctl(fd, I2C_SLAVE, OLED_ADDR);

    oled_init();

    sleep(1);

    oled_fill();

    return 0;
}
