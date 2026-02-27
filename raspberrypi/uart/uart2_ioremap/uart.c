#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/kthread.h>
#include <linux/delay.h>

/* ---------- UART2 PL011 ---------- */

#define UART2_BASE 0xFE201400
#define UART_SIZE  0x90

#define UART_DR    0x00
#define UART_FR    0x18
#define UART_IBRD  0x24
#define UART_FBRD  0x28
#define UART_LCRH  0x2C
#define UART_CR    0x30
#define UART_ICR   0x44

#define FR_TXFF (1<<5)
#define FR_RXFE (1<<4)

static void __iomem *uart2;
static struct task_struct *loop_thread;

/* ---------- TX ---------- */
static void uart_putc(char c)
{
    while (readl(uart2 + UART_FR) & FR_TXFF)
        cpu_relax();

    writel(c, uart2 + UART_DR);
}

/* ---------- RX ---------- */
static char uart_getc(void)
{
    while (readl(uart2 + UART_FR) & FR_RXFE)
        cpu_relax();

    return readl(uart2 + UART_DR) & 0xFF;
}

static void uart_puts(const char *s)
{
    while (*s)
        uart_putc(*s++);
}

/* ---------- LOOPBACK THREAD ---------- */
static int uart_loop(void *data)
{
    char tx='A', rx;

    pr_info("UART2 loopback thread running\n");

    while (!kthread_should_stop())
    {
        pr_info("TX -> %c\n", tx);

        uart_putc(tx);

        msleep(300);

        rx = uart_getc();

        pr_info("RX <- %c\n", rx);

        tx++;
        if(tx>'Z') tx='A';

        msleep(700);
    }

    return 0;
}

/* ---------- INIT ---------- */
static int __init uart2_init(void)
{
    pr_info("UART2 ioremap driver loading\n");

    uart2 = ioremap(UART2_BASE, UART_SIZE);
    if (!uart2)
        return -ENOMEM;

    pr_info("UART2 mapped @ %p\n", uart2);

    /* Disable UART */
    writel(0x0, uart2 + UART_CR);

    /* Clear interrupts */
    writel(0x7FF, uart2 + UART_ICR);

    /*
     * Baud =115200
     * UARTCLK=48MHz
     */
    writel(26, uart2 + UART_IBRD);
    writel(3,  uart2 + UART_FBRD);

    /* 8N1 + FIFO */
    writel((1<<4)|(1<<5)|(1<<6),
           uart2 + UART_LCRH);

    /* Enable TX + RX + UART */
    writel((1<<0)|(1<<8)|(1<<9),
           uart2 + UART_CR);

    pr_info("UART2 initialized\n");

    uart_puts("\r\nUART2 LOOPBACK START\r\n");

    loop_thread = kthread_run(uart_loop,NULL,"uart2_loop");

    return 0;
}

/* ---------- EXIT ---------- */
static void __exit uart2_exit(void)
{
    pr_info("Stopping UART2 driver\n");

    if(loop_thread)
        kthread_stop(loop_thread);

    writel(0x0, uart2 + UART_CR);

    if(uart2)
        iounmap(uart2);

    pr_info("UART2 driver unloaded\n");
}

module_init(uart2_init);
module_exit(uart2_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Karthik");
MODULE_DESCRIPTION("Raspberry Pi4 UART2 ioremap loopback driver");
