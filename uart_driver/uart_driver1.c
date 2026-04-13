#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/delay.h>

#define UART0_BASE_PHYS   0xFE201000
#define UART0_SIZE        0x90

/* UART Registers */
#define UART_DR     0x00
#define UART_FR     0x18
#define UART_IBRD   0x24
#define UART_FBRD   0x28
#define UART_LCRH   0x2C
#define UART_CR     0x30
#define UART_IMSC   0x38
#define UART_ICR    0x44

/* FR bits */
#define FR_TXFF     (1 << 5)
#define FR_RXFE     (1 << 4)

static void __iomem *uart_base;

/* Polling TX */
static void uart_putc(char c)
{
    while (readl(uart_base + UART_FR) & FR_TXFF)
        cpu_relax();

    writel(c, uart_base + UART_DR);
}

/* Polling RX */
static char uart_getc(void)
{
    while (readl(uart_base + UART_FR) & FR_RXFE)
        cpu_relax();

    return readl(uart_base + UART_DR) & 0xFF;
}

static void uart_puts(const char *s)
{
    while (*s)
        uart_putc(*s++);
}

static int __init uart_init(void)
{
    pr_info("UART ioremap driver init\n");

    uart_base = ioremap(UART0_BASE_PHYS, UART0_SIZE);
    if (!uart_base) {
        pr_err("UART ioremap failed\n");
        return -ENOMEM;
    }

    /* Disable UART */
    writel(0x0, uart_base + UART_CR);

    /* Clear interrupts */
    writel(0x7FF, uart_base + UART_ICR);

    /*
     * Baud rate = 115200
     * UARTCLK = 48MHz
     * Divider = 48,000,000 / (16 * 115200) = 26.0416
     */
    writel(26, uart_base + UART_IBRD);
    writel(3,  uart_base + UART_FBRD);

    /* 8N1, FIFO enabled */
    writel((1 << 4) | (1 << 5) | (1 << 6),
           uart_base + UART_LCRH);

    /* Enable UART, TX, RX */
    writel((1 << 0) | (1 << 8) | (1 << 9),
           uart_base + UART_CR);

    uart_puts("\n\rUART ioremap driver active\r\n");

    /* Echo test */
    uart_puts("Type a character...\r\n");

    {
        char c = uart_getc();
        uart_puts("You typed: ");
        uart_putc(c);
        uart_puts("\r\n");
    }

    return 0;
}

static void __exit uart_exit(void)
{
    uart_puts("UART driver exit\r\n");

    writel(0x0, uart_base + UART_CR);

    if (uart_base)
        iounmap(uart_base);

    pr_info("UART ioremap driver unloaded\n");
}

module_init(uart_init);
module_exit(uart_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("SHARANG");
MODULE_DESCRIPTION("BCM2711 UART driver using ioremap only");
