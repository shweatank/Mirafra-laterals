#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/io.h>
#include <linux/uaccess.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/slab.h>

/* ============================================================
 * Raspberry Pi 4 - UART2 Character Driver
 * ============================================================
 */

/* ---------- UART2 ---------- */
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
#define GPIO_BASE  0xFE200000
#define GPIO_SIZE  0x100
#define GPFSEL0    0x00

/* ---------- Device ---------- */
#define DEVICE_NAME "uart2"
#define CLASS_NAME  "uart2_class"

static dev_t dev_num;
static struct cdev uart_cdev;
static struct class *uart_class;

static void __iomem *uart2;
static void __iomem *gpio;

/* ============================================================
 * Low-Level UART Functions
 * ============================================================
 */

static void uart_putc(char c)
{
    while (readl(uart2 + UART_FR) & FR_TXFF)
        cpu_relax();

    writel(c, uart2 + UART_DR);
}

static char uart_getc(void)
{
    while (readl(uart2 + UART_FR) & FR_RXFE)
        cpu_relax();

    return readl(uart2 + UART_DR) & 0xFF;
}

/* ============================================================
 * File Operations
 * ============================================================
 */

static int uart_open(struct inode *inode, struct file *file)
{
    pr_info("uart2: device opened\n");
    return 0;
}

static int uart_release(struct inode *inode, struct file *file)
{
    pr_info("uart2: device closed\n");
    return 0;
}

static ssize_t uart_write(struct file *file,
                          const char __user *buf,
                          size_t len,
                          loff_t *offset)
{
    size_t i;
    char *kbuf;

    kbuf = kmalloc(len, GFP_KERNEL);
    if (!kbuf)
        return -ENOMEM;

    if (copy_from_user(kbuf, buf, len)) {
        kfree(kbuf);
        return -EFAULT;
    }

    for (i = 0; i < len; i++)
        uart_putc(kbuf[i]);

    kfree(kbuf);
    return len;
}

static ssize_t uart_read(struct file *file,
                         char __user *buf,
                         size_t len,
                         loff_t *offset)
{
    char c;

    /* If no data available, return 0 */
    if (readl(uart2 + UART_FR) & FR_RXFE)
        return 0;

    c = readl(uart2 + UART_DR) & 0xFF;

    if (copy_to_user(buf, &c, 1))
        return -EFAULT;

    return 1;
}

static struct file_operations fops = {
    .owner   = THIS_MODULE,
    .open    = uart_open,
    .release = uart_release,
    .read    = uart_read,
    .write   = uart_write,
};

/* ============================================================
 * GPIO ALT4 Configuration
 * ============================================================
 */

static int gpio_uart2_config(void)
{
    u32 val;

    gpio = ioremap(GPIO_BASE, GPIO_SIZE);
    if (!gpio)
        return -ENOMEM;

    val = readl(gpio + GPFSEL0);

    val &= ~(0x7 << 0);
    val &= ~(0x7 << 3);

    val |= (0x3 << 0);  // ALT4
    val |= (0x3 << 3);

    writel(val, gpio + GPFSEL0);

    return 0;
}

/* ============================================================
 * Module Init
 * ============================================================
 */

static int __init uart2_init(void)
{
    int ret;

    pr_info("uart2: loading driver\n");

    /* Allocate device number */
    ret = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
    if (ret < 0)
        return ret;

    cdev_init(&uart_cdev, &fops);
    uart_cdev.owner = THIS_MODULE;

    ret = cdev_add(&uart_cdev, dev_num, 1);
    if (ret)
        goto unregister;

    uart_class = class_create(CLASS_NAME);
    device_create(uart_class, NULL, dev_num, NULL, DEVICE_NAME);

    /* Map UART */
    uart2 = ioremap(UART2_BASE, UART_SIZE);
    if (!uart2) {
        ret = -ENOMEM;
        goto device_destroy;
    }

    /* Configure GPIO */
    gpio_uart2_config();

    /* Disable UART */
    writel(0x0, uart2 + UART_CR);
    while (readl(uart2 + UART_FR) & FR_BUSY)
        cpu_relax();

    writel(0x7FF, uart2 + UART_ICR);

    /* 115200 @ 48MHz */
    writel(26, uart2 + UART_IBRD);
    writel(3,  uart2 + UART_FBRD);

    writel((1<<4)|(1<<5)|(1<<6), uart2 + UART_LCRH);
    writel((1<<0)|(1<<8)|(1<<9), uart2 + UART_CR);

    pr_info("uart2: driver ready (/dev/uart2)\n");
    return 0;

device_destroy:
    device_destroy(uart_class, dev_num);
    class_destroy(uart_class);
    cdev_del(&uart_cdev);

unregister:
    unregister_chrdev_region(dev_num, 1);
    return ret;
}

/* ============================================================
 * Module Exit
 * ============================================================
 */

static void __exit uart2_exit(void)
{
    writel(0x0, uart2 + UART_CR);

    iounmap(uart2);
    iounmap(gpio);

    device_destroy(uart_class, dev_num);
    class_destroy(uart_class);
    cdev_del(&uart_cdev);
    unregister_chrdev_region(dev_num, 1);

    pr_info("uart2: driver unloaded\n");
}

module_init(uart2_init);
module_exit(uart2_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("sahil");
MODULE_DESCRIPTION("RPi4 UART2 Character Driver");
