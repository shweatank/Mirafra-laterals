#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/delay.h>

#define UART0_BASE_PHYS   0xFE201000
#define UART0_SIZE        0x90

#define UART_DR     0x00
#define UART_FR     0x18
#define UART_IBRD   0x24
#define UART_FBRD   0x28
#define UART_LCRH   0x2C
#define UART_CR     0x30
#define UART_ICR    0x44

#define FR_TXFF     (1 << 5)
#define FR_RXFE     (1 << 4)

static void __iomem *uart_base;

/* TX */
static void uart_putc(char c)
{
    while (readl(uart_base + UART_FR) & FR_TXFF)
        cpu_relax();

    writel(c, uart_base + UART_DR);
}

/* Non-blocking RX check */
static int uart_rx_available(void)
{
    return !(readl(uart_base + UART_FR) & FR_RXFE);
}

static char uart_getc(void)
{
    return readl(uart_base + UART_DR) & 0xFF;
}

static void uart_puts(const char *s)
{
    while (*s)
        uart_putc(*s++);
}

/* Flush RX FIFO */
static void uart_flush_rx(void)
{
    while (!(readl(uart_base + UART_FR) & FR_RXFE))
        readl(uart_base + UART_DR);
}

static int __init uart_init(void)
{
    int i;

    pr_info("UART polling driver init\n");

    uart_base = ioremap(UART0_BASE_PHYS, UART0_SIZE);
    if (!uart_base)
        return -ENOMEM;

    /* Disable UART */
    writel(0x0, uart_base + UART_CR);
    writel(0x7FF, uart_base + UART_ICR);

    /* 115200 @ 48MHz */
    writel(26, uart_base + UART_IBRD);
    writel(3,  uart_base + UART_FBRD);

    writel((1 << 4) | (1 << 5) | (1 << 6),
           uart_base + UART_LCRH);

    writel((1 << 0) | (1 << 8) | (1 << 9),
           uart_base + UART_CR);

    uart_puts("\r\nUART polling active\r\n");

    /* IMPORTANT: flush loopback garbage */
    uart_flush_rx();

    pr_info("Start polling for characters...\n");

    /* Poll for some time (example: 10 seconds) */
    for (i = 0; i < 10000; i++) {

        if (uart_rx_available()) {
            char c = uart_getc();

            pr_info("RX: %c (0x%02x)\n", c, c);

            uart_putc(c);  /* echo */
        }

        udelay(1000);  /* 1ms delay */
    }

    pr_info("Polling finished\n");

    return 0;
}

static void __exit uart_exit(void)
{
    pr_info("UART driver exit\n");

    writel(0x0, uart_base + UART_CR);

    if (uart_base)
        iounmap(uart_base);
}

module_init(uart_init);
module_exit(uart_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Prashant");
MODULE_DESCRIPTION("BCM2711 UART polling driver with RX flush");