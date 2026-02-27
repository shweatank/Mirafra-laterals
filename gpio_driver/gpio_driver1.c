#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/io.h>

#define DEVICE_NAME "gpio_user"
#define CLASS_NAME  "uart_user_class"
#define DRV_NAME    "uart_ioctl_drv"

#define UART0_BASE_PHYS 0xFE201000
#define UART0_SIZE      0x90

#define UART_DR   0x00
#define UART_FR   0x18
#define UART_CR   0x30

#define FR_TXFF   (1 << 5)
#define FR_RXFE   (1 << 4)

#define CALC_IOC_MAGIC 'B'

struct uart_req {
    char tx_buf[100];
    char rx_buf[100];
    int  len;
};

#define UART_IOC_TXRX _IOWR(CALC_IOC_MAGIC, 1, struct uart_req)

static dev_t g_dev;
static struct cdev g_cdev;
static struct class *g_class;
static struct device *g_device;
static void __iomem *uart_base;

/* UART TX */
static void uart_putc(char c)
{
    while (readl(uart_base + UART_FR) & FR_TXFF)
        cpu_relax();
    writel(c, uart_base + UART_DR);
}

/* UART RX */
static char uart_getc(void)
{
    while (readl(uart_base + UART_FR) & FR_RXFE)
        cpu_relax();
    return readl(uart_base + UART_DR) & 0xFF;
}

static long uart_ioctl(struct file *file,
                       unsigned int cmd,
                       unsigned long arg)
{
    struct uart_req req;
    int i;

    if (cmd != UART_IOC_TXRX)
        return -EINVAL;

    if (copy_from_user(&req, (void __user *)arg, sizeof(req)))
        return -EFAULT;

    /* Transmit to UART */
    for (i = 0; i < req.len; i++)
        uart_putc(req.tx_buf[i]);

    /* Receive from UART */
    for (i = 0; i < req.len; i++)
        req.rx_buf[i] = uart_getc();

    if (copy_to_user((void __user *)arg, &req, sizeof(req)))
        return -EFAULT;

    return 0;
}

static const struct file_operations fops = {
    .owner          = THIS_MODULE,
    .unlocked_ioctl = uart_ioctl,
};

static int __init uart_drv_init(void)
{
    alloc_chrdev_region(&g_dev, 0, 1, DEVICE_NAME);
    cdev_init(&g_cdev, &fops);
    cdev_add(&g_cdev, g_dev, 1);

    g_class = class_create(CLASS_NAME);
    g_device = device_create(g_class, NULL, g_dev, NULL, DEVICE_NAME);

    uart_base = ioremap(UART0_BASE_PHYS, UART0_SIZE);

    /* Enable UART (assumes already configured) */
    writel((1 << 0) | (1 << 8) | (1 << 9),
           uart_base + UART_CR);

    pr_info("UART ioctl driver loaded\n");
    return 0;
}

static void __exit uart_drv_exit(void)
{
    iounmap(uart_base);
    device_destroy(g_class, g_dev);
    class_destroy(g_class);
    cdev_del(&g_cdev);
    unregister_chrdev_region(g_dev, 1);
    pr_info("UART ioctl driver unloaded\n");
}

module_init(uart_drv_init);
module_exit(uart_drv_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Modified for UART TXRX");
MODULE_DESCRIPTION("User <-> Kernel <-> UART IOCTL Driver");
