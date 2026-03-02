// simple_gpio.c
#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#include <linux/mutex.h>

#define DRIVER_NAME "simple_gpio"
#define DEVICE_NAME "simple_gpio"

#define GPIO_BASE   0xFE200000   // BCM2711 (Raspberry Pi 4)
#define GPIO_SIZE   0xB4

#define GPFSEL1     0x04
#define GPSET0      0x1C
#define GPCLR0      0x28

#define GPIO17_SHIFT 21   // GPIO17 function select bits

static dev_t dev_num;
static struct cdev gpio_cdev;
static struct class *gpio_class;
static struct device *gpio_device;

static void __iomem *gpio_base;

static DEFINE_MUTEX(gpio_mutex);   // Critical section protection

/* Configure GPIO17 as output */
static void gpio_set_output(void)
{
    u32 val;

    val = readl(gpio_base + GPFSEL1);
    val &= ~(0x7 << GPIO17_SHIFT);
    val |=  (0x1 << GPIO17_SHIFT);
    writel(val, gpio_base + GPFSEL1);
}

/* Write handler */
static ssize_t gpio_write(struct file *file,
                          const char __user *buf,
                          size_t len, loff_t *off)
{
    char kbuf;

 

    mutex_lock(&gpio_mutex);
  if (copy_from_user(&kbuf, buf, 1))
        return -EFAULT;
   pr_info("entered critical section");
   if (kbuf == '1')
        writel(1 << 17, gpio_base + GPSET0);
    else if (kbuf == '0')
        writel(1 << 17, gpio_base + GPCLR0);

    pr_info("GPIO17 set to %c by PID %d\n", kbuf, current->pid);
   
    mutex_unlock(&gpio_mutex);
   pr_info("exited critical section");
    return len;
}

static const struct file_operations gpio_fops = {
    .owner = THIS_MODULE,
    .write = gpio_write,
};

static int __init simple_gpio_init(void)
{
    int ret;

    /* Map GPIO registers */
    gpio_base = ioremap(GPIO_BASE, GPIO_SIZE);
    if (!gpio_base) {
        pr_err("Failed to map GPIO memory\n");
        return -ENOMEM;
    }

    gpio_set_output();

    /* Allocate char device */
    ret = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
    if (ret) {
        pr_err("Failed to allocate chrdev\n");
        iounmap(gpio_base);
        return ret;
    }

    cdev_init(&gpio_cdev, &gpio_fops);
    ret = cdev_add(&gpio_cdev, dev_num, 1);
    if (ret) {
        unregister_chrdev_region(dev_num, 1);
        iounmap(gpio_base);
        return ret;
    }

    /* Create class (Linux 6.x API) */
    gpio_class = class_create(DEVICE_NAME);
    if (IS_ERR(gpio_class)) {
        cdev_del(&gpio_cdev);
        unregister_chrdev_region(dev_num, 1);
        iounmap(gpio_base);
        return PTR_ERR(gpio_class);
    }

    gpio_device = device_create(gpio_class, NULL, dev_num, NULL, DEVICE_NAME);
    if (IS_ERR(gpio_device)) {
        class_destroy(gpio_class);
        cdev_del(&gpio_cdev);
        unregister_chrdev_region(dev_num, 1);
        iounmap(gpio_base);
        return PTR_ERR(gpio_device);
    }

    pr_info("Simple GPIO driver loaded (Linux 6.12)\n");
    return 0;
}

static void __exit simple_gpio_exit(void)
{
    device_destroy(gpio_class, dev_num);
    class_destroy(gpio_class);
    cdev_del(&gpio_cdev);
    unregister_chrdev_region(dev_num, 1);

    iounmap(gpio_base);

    pr_info("Simple GPIO driver removed\n");
}

module_init(simple_gpio_init);
module_exit(simple_gpio_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Isshika Sinha");
MODULE_DESCRIPTION("Simple GPIO Driver with Mutex (Linux 6.12)");
