#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/kthread.h>
#include <linux/delay.h>
#include <linux/err.h>

/* ============================================================
 * Raspberry Pi 4 - BCM2711
 * UART2 (PL011) + GPIO ALT4 configuration
 * ============================================================
 */

/* ---------- UART2 PL011 ---------- */

#define UART2_BASE 0xFE201400
#define UART_SIZE  0x100

#define UART_DR    0x00
#define UART_FR    0x18
#define UART_IBRD  0x24
#define UART_FBRD  0x28
#define UART_LCRH  0x2C
#define UART_CR    0x30
#define UART_ICR   0x44

#define FR_TXFF (1<<5)
#define FR_RXFE (1<<4)
#define FR_BUSY (1<<3)

/* ---------- GPIO ---------- */

#define GPIO_BASE   0xFE200000
#define GPIO_SIZE   0x100
#define GPFSEL0     0x00

/* ============================================================ */

static void __iomem *uart2;
static void __iomem *gpio;
static struct task_struct *loop_thread;

/* ============================================================
 * GPIO CONFIGURATION (ALT4 for UART2)
 * ============================================================
 */
static int gpio_uart2_config(void)
{
    u32 val;

    gpio = ioremap(GPIO_BASE, GPIO_SIZE);
    if (!gpio) {
        pr_err("Failed to map GPIO\n");
        return -ENOMEM;
    }

    val = readl(gpio + GPFSEL0);

    /* Clear GPIO0 (bits 2:0) */
    val &= ~(0x7 << 0);

    /* Clear GPIO1 (bits 5:3) */
    val &= ~(0x7 << 3);

    /* ALT4 = 011 */
    val |= (0x3 << 0);  // GPIO0 -> UART2_TX
    val |= (0x3 << 3);  // GPIO1 -> UART2_RX

    writel(val, gpio + GPFSEL0);

    pr_info("GPIO0/1 set to ALT4 (UART2)\n");
    return 0;
}

/* ============================================================
 * UART LOW-LEVEL FUNCTIONS
 * ============================================================
 */

static void uart_putc(char c)
{
    while (readl(uart2 + UART_FR) & FR_TXFF)
        cpu_relax();

    writel(c, uart2 + UART_DR);
}

static int uart_getc_timeout(char *c)
{
    int timeout = 1000000;

    while ((readl(uart2 + UART_FR) & FR_RXFE) && timeout--)
        cpu_relax();

    if (timeout <= 0)
        return -ETIMEDOUT;

    *c = readl(uart2 + UART_DR) & 0xFF;
    return 0;
}

static void uart_puts(const char *s)
{
    while (*s)
        uart_putc(*s++);
}

/* ============================================================
 * LOOPBACK THREAD
 * ============================================================
 */

static int uart_loop(void *data)
{
    char tx = 'A';
    char rx;

    pr_info("UART2 loopback thread started\n");

    while (!kthread_should_stop())
    {
        pr_info("TX -> %c\n", tx);
        uart_putc(tx);

        msleep(300);

        if (uart_getc_timeout(&rx) == 0)
            pr_info("RX <- %c\n", rx);
        else
            pr_warn("RX timeout\n");

        tx++;
        if (tx > 'Z')
            tx = 'A';

        msleep(700);
    }

    return 0;
}

/* ============================================================
 * MODULE INIT
 * ============================================================
 */

static int __init uart2_init(void)
{
    int ret;

    pr_info("UART2 driver loading\n");

    /* Map UART2 */
    uart2 = ioremap(UART2_BASE, UART_SIZE);
    if (!uart2)
        return -ENOMEM;

    /* Configure GPIO ALT4 */
    ret = gpio_uart2_config();
    if (ret)
        goto err_uart;

    /* Disable UART */
    writel(0x0, uart2 + UART_CR);

    /* Wait until not busy */
    while (readl(uart2 + UART_FR) & FR_BUSY)
        cpu_relax();

    /* Clear interrupts */
    writel(0x7FF, uart2 + UART_ICR);

    /*
     * Baud = 115200
     * UARTCLK = 48 MHz
     * Divider = 48,000,000 / (16 * 115200) = 26.0417
     * IBRD = 26
     * FBRD = 3
     */
    writel(26, uart2 + UART_IBRD);
    writel(3,  uart2 + UART_FBRD);

    /* 8N1 + FIFO enable */
    writel((1<<4)|(1<<5)|(1<<6), uart2 + UART_LCRH);

    /* Enable UART, TX, RX */
    writel((1<<0)|(1<<8)|(1<<9), uart2 + UART_CR);
    wmb();

    pr_info("UART2 initialized @115200 8N1\n");

    uart_puts("\r\nUART2 LOOPBACK START\r\n");

    /* Start thread */
    loop_thread = kthread_run(uart_loop, NULL, "uart2_loop");
    if (IS_ERR(loop_thread)) {
        pr_err("Failed to create thread\n");
        ret = PTR_ERR(loop_thread);
        goto err_gpio;
    }

    return 0;

err_gpio:
    if (gpio)
        iounmap(gpio);

err_uart:
    if (uart2)
        iounmap(uart2);

    return ret;
}

/* ============================================================
 * MODULE EXIT
 * ============================================================
 */

static void __exit uart2_exit(void)
{
    pr_info("Stopping UART2 driver\n");

    if (loop_thread)
        kthread_stop(loop_thread);

    /* Disable UART */
    if (uart2)
        writel(0x0, uart2 + UART_CR);

    if (gpio)
        iounmap(gpio);

    if (uart2)
        iounmap(uart2);

    pr_info("UART2 driver unloaded\n");
}

module_init(uart2_init);
module_exit(uart2_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sahil");
MODULE_DESCRIPTION("Raspberry Pi 4 UART2 PL011 loopback driver with GPIO ALT4");
