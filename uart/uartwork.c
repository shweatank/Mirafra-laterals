#include <linux/module.h>
#include <linux/fs.h>
#include <linux/io.h>
#include <linux/uaccess.h>
#include <linux/device.h>
#include <linux/delay.h>

#define DEVICE_NAME "uartdriver"
#define CLASS_NAME  "uart_class"

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

static int major;
static struct class *uart_class;
static struct device *uart_device;

static void gpio_uart_init(void)
{
    u32 val;

    val = readl(gpio + GPFSEL1);
    val &= ~((7<<12) | (7<<15));
    val |=  (4<<12) | (4<<15);
    writel(val, gpio + GPFSEL1);

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

static char uart_getc(void)
{
    while (readl(uart + UART_FR) & FR_RXFE);
    return readl(uart + UART_DR) & 0xFF;
}

static int uart_open(struct inode *inode, struct file *file)
{
    pr_info("UART device opened\n");
    return 0;
}

static int uart_release(struct inode *inode, struct file *file)
{
    pr_info("UART device closed\n");
    return 0;
}

static ssize_t uart_write(struct file *file,
                          const char __user *buf,
                          size_t len, loff_t *offset)
{
    char kbuf[256];
    int i;

    if (len > sizeof(kbuf))
        len = sizeof(kbuf);

    if (copy_from_user(kbuf, buf, len))
        return -EFAULT;

    for (i = 0; i < len; i++)
        uart_putc(kbuf[i]);

    return len;
}

static ssize_t uart_read(struct file *file,
                         char __user *buf,
                         size_t len, loff_t *offset)
{
    char c;

    if (len < 1)
        return 0;

    c = uart_getc();

    if (copy_to_user(buf, &c, 1))
        return -EFAULT;

    return 1;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = uart_open,
    .release = uart_release,
    .read = uart_read,
    .write = uart_write,
};

static int __init uart_init(void)
{
    pr_info("UART Char Driver Init\n");

    uart = ioremap(UART0_BASE, UART_SIZE);
    gpio = ioremap(GPIO_BASE, 0x100);

    if (!uart || !gpio)
        return -ENOMEM;

    gpio_uart_init();

    writel(0, uart + UART_CR);
    writel(0x7FF, uart + UART_ICR);
    writel(26, uart + UART_IBRD);
    writel(3, uart + UART_FBRD);
    writel((1<<4)|(1<<5)|(1<<6), uart + UART_LCRH);
    writel((1<<0)|(1<<8)|(1<<9), uart + UART_CR);

    major = register_chrdev(0, DEVICE_NAME, &fops);

    uart_class = class_create(CLASS_NAME);
    uart_device = device_create(uart_class, NULL,
                                MKDEV(major,0),
                                NULL, DEVICE_NAME);

    pr_info("Device created: /dev/%s\n", DEVICE_NAME);
    return 0;
}

static void __exit uart_exit(void)
{
    device_destroy(uart_class, MKDEV(major,0));
    class_destroy(uart_class);
    unregister_chrdev(major, DEVICE_NAME);

    if (uart)
        iounmap(uart);
    if (gpio)
        iounmap(gpio);

    pr_info("UART driver removed\n");
}

module_init(uart_init);
module_exit(uart_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Isshika Sinha");
