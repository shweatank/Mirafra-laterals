#include <linux/module.h>
#include <linux/io.h>
#include <linux/delay.h>

#define UART0_BASE  0xFE201000
#define GPIO_BASE   0xFE200000

#define UART_SIZE   0x90

/* UART registers */
#define UART_DR   0x00
#define UART_FR   0x18
#define UART_IBRD 0x24
#define UART_FBRD 0x28
#define UART_LCRH 0x2C
#define UART_CR   0x30
#define UART_ICR  0x44

#define FR_TXFF (1 << 5)
#define FR_RXFE (1 << 4)

/* GPIO */
#define GPFSEL1   0x04
#define GPPUD     0x94
#define GPPUDCLK0 0x98

static void __iomem *uart;
static void __iomem *gpio;

static void gpio_uart_init(void)
{
    u32 val;

    /* GPIO14 & 15 -> ALT0 (TXD0/RXD0) */
    val = readl(gpio + GPFSEL1);
    val &= ~((7<<12) | (7<<15));
    val |=  (4<<12) | (4<<15);
    writel(val, gpio + GPFSEL1);

    /* disable pull-up/down */
    writel(0, gpio + GPPUD);
    udelay(5);
    writel((1<<14)|(1<<15), gpio + GPPUDCLK0);
    udelay(5);
    writel(0, gpio + GPPUDCLK0);
}

static void uart_putc(char c)
{
    while (readl(uart + UART_FR) & FR_TXFF);
    writel(c, uart + UART_DR);
}

static int uart_getc_timeout(char *c)
{
    int timeout = 1000;

    while (timeout--) {
        if (!(readl(uart + UART_FR) & FR_RXFE)) {
            *c = readl(uart + UART_DR) & 0xFF;
            return 0;
        }
        udelay(1000);
    }
    return -1;
}

static void uart_puts(const char *s)
{
    while (*s)
        uart_putc(*s++);
}

static int __init uart_init(void)
{
    char rx;

    pr_info("PL011 UART loopback test\n");

    uart = ioremap(UART0_BASE, UART_SIZE);
    gpio = ioremap(GPIO_BASE, 0x100);

    if (!uart || !gpio)
        return -ENOMEM;

    gpio_uart_init();

    /* disable UART */
    writel(0, uart + UART_CR);

    writel(0x7FF, uart + UART_ICR);

    /* baud 115200 */
    writel(26, uart + UART_IBRD);
    writel(3,  uart + UART_FBRD);

    /* 8N1 FIFO */
    writel((1<<4)|(1<<5)|(1<<6), uart + UART_LCRH);

    /* enable UART TX RX */
    writel((1<<0)|(1<<8)|(1<<9), uart + UART_CR);

    uart_puts("\r\nLoopback Start\r\n");

    uart_putc('A');

    if (uart_getc_timeout(&rx) == 0) {
        uart_puts("Received: ");
        uart_putc(rx);
        uart_puts("\r\nSUCCESS\r\n");
	pr_info("Received char: %c(0x%02X)\n", rx,rx);
        pr_info("Loopback OK\n");
    } else {
        uart_puts("Loopback FAIL\r\n");
        pr_err("No data received\n");
    }

    return 0;
}

static void __exit uart_exit(void)
{
    if (uart)
        iounmap(uart);
    if (gpio)
        iounmap(gpio);

    pr_info("UART module removed\n");
}

module_init(uart_init);
module_exit(uart_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Jerin Jose");
MODULE_DESCRIPTION("Uart loopback with gpio configuration");
