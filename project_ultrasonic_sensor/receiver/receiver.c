#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>
#include <termios.h>

#define OLED_ADDR 0x3C
#define WIDTH 128
#define HEIGHT 64

int i2c_fd;
unsigned char oled_buf[WIDTH * HEIGHT / 8];

/* ---------------- FUNCTION PROTOTYPES ---------------- */
void oled_draw_char_big(int x, int page, char c);
void oled_print_big(int x, int page, char *str);
void oled_print_big_center(char *str, int page);

/* ---------------- UART ---------------- */
void uart_init(int fd)
{
    struct termios tty;
    tcgetattr(fd, &tty);

    cfsetospeed(&tty, B9600);
    cfsetispeed(&tty, B9600);

    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;
    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;

    tty.c_lflag = 0;
    tty.c_oflag = 0;
    tty.c_iflag = 0;

    tty.c_cc[VMIN]  = 1;
    tty.c_cc[VTIME] = 1;

    tcsetattr(fd, TCSANOW, &tty);
}

/* ---------------- OLED LOW LEVEL ---------------- */
void cmd(unsigned char c) { unsigned char buf[2]={0x00,c}; write(i2c_fd,buf,2);}
void data(unsigned char d){ unsigned char buf[2]={0x40,d}; write(i2c_fd,buf,2); }

void oled_init()
{
    cmd(0xAE); cmd(0x20); cmd(0x00); cmd(0xB0); cmd(0xC8);
    cmd(0x00); cmd(0x10); cmd(0x40); cmd(0x81); cmd(0xFF);
    cmd(0xA1); cmd(0xA6); cmd(0xA8); cmd(0x3F); cmd(0xA4);
    cmd(0xD3); cmd(0x00); cmd(0xD5); cmd(0xF0); cmd(0xD9); cmd(0x22);
    cmd(0xDA); cmd(0x12); cmd(0xDB); cmd(0x20); cmd(0x8D); cmd(0x14); cmd(0xAF);
}

void oled_clear_buf(){ memset(oled_buf,0,sizeof(oled_buf)); }

void oled_update()
{
    for(int page=0; page<8; page++){
        cmd(0xB0+page); cmd(0x00); cmd(0x10);
        for(int col=0; col<WIDTH; col++) data(oled_buf[page*WIDTH+col]);
    }
}

/* ---------------- FONT ---------------- */
unsigned char font5x7[][5]={
    ['0']={0x3E,0x51,0x49,0x45,0x3E}, ['1']={0x00,0x42,0x7F,0x40,0x00},
    ['2']={0x42,0x61,0x51,0x49,0x46}, ['3']={0x21,0x41,0x45,0x4B,0x31},
    ['4']={0x18,0x14,0x12,0x7F,0x10}, ['5']={0x27,0x45,0x45,0x45,0x39},
    ['6']={0x3C,0x4A,0x49,0x49,0x30}, ['7']={0x01,0x71,0x09,0x05,0x03},
    ['8']={0x36,0x49,0x49,0x49,0x36}, ['9']={0x06,0x49,0x49,0x29,0x1E},
    ['c']={0x38,0x44,0x44,0x44,0x28}, ['m']={0x7C,0x04,0x18,0x04,0x78},
    [' ']={0,0,0,0,0}
};

/* ---------------- BIG FONT DRAW ---------------- */
void oled_draw_char_big(int x, int page, char c)
{
    unsigned char *ch = font5x7[(int)c];
    for(int col=0; col<5; col++){
        unsigned char line = ch[col];
        for(int row=0; row<7; row++){
            if(line & (1<<row)){
                int y = page*8 + row*2;
                for(int dx=0; dx<2; dx++) for(int dy=0; dy<2; dy++){
                    int px=x+col*2+dx; int py=y+dy;
                    if(px<WIDTH && py<HEIGHT) oled_buf[(py/8)*WIDTH+px] |= (1<<(py%8));
                }
            }
        }
    }
}

void oled_print_big(int x, int page, char *str)
{
    while(*str){
        oled_draw_char_big(x,page,*str);
        x += 12;
        str++;
    }
}

void oled_print_big_center(char *str, int page)
{
    int len = strlen(str);
    int total_width = len*12;
    int x_start = (WIDTH - total_width)/2;
    if(x_start<0) x_start=0;
    oled_print_big(x_start,page,str);
}

/* ---------------- MAIN ---------------- */
int main()
{
    int uart_fd;
    char buffer[128];
    int idx=0;
    char rxbuf[32];
    int n;

    i2c_fd = open("/dev/i2c-1", O_RDWR);
    ioctl(i2c_fd, I2C_SLAVE, OLED_ADDR);
    oled_init();
    oled_clear_buf();
    oled_update();

    uart_fd = open("/dev/serial0", O_RDWR | O_NOCTTY);
    uart_init(uart_fd);

    while(1){
        n = read(uart_fd, rxbuf, sizeof(rxbuf));
        if(n>0){
            for(int i=0;i<n;i++){
                char ch = rxbuf[i];
                if(ch=='\n'){
                    buffer[idx]='\0';
                    if(idx>0){
                        printf("Received: %s\n", buffer);
                        oled_clear_buf();
                        oled_print_big_center(buffer,2);
                        oled_update();
                    }
                    idx=0;
                } else if(ch>=32 && ch<=126){
                    if(idx<sizeof(buffer)-1) buffer[idx++]=ch;
                }
            }
        }
    }
}
