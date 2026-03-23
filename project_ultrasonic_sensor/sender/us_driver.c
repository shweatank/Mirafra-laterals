#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/timekeeping.h>
#include <linux/delay.h>
#include <linux/io.h>

#define DEVICE "ultra_dev"

/* BCM2711 base */
#define GPIO_BASE 0xFE200000
#define GPFSEL0 0x00 
#define GPFSEL1 0x04
#define GPFSEL2 0x08
#define GPSET0  0x1C
#define GPCLR0  0x28
#define GPLEV0  0x34

#define TRIG 17
#define ECHO 27

static void __iomem *gpio_base;
static int major;

/* GPIO helpers */
static void gpio_set_output(int pin)
{
    u32 reg = (pin < 10) ? GPFSEL0 :
              (pin < 20) ? GPFSEL1 : GPFSEL2;

    u32 shift = (pin % 10) * 3;

    u32 val = ioread32(gpio_base + reg);
    val &= ~(7 << shift);
    val |= (1 << shift);
    iowrite32(val, gpio_base + reg);
}

static void gpio_set_input(int pin)
{
    u32 reg = (pin < 10) ? GPFSEL0 :
              (pin < 20) ? GPFSEL1 : GPFSEL2;

    u32 shift = (pin % 10) * 3;

    u32 val = ioread32(gpio_base + reg);
    val &= ~(7 << shift);
    iowrite32(val, gpio_base + reg);
}

static void gpio_set(int pin)
{
    iowrite32(1 << pin, gpio_base + GPSET0);
}

static void gpio_clear(int pin)
{
    iowrite32(1 << pin, gpio_base + GPCLR0);
}

static int gpio_read(int pin)
{
    return (ioread32(gpio_base + GPLEV0) & (1 << pin)) ? 1 : 0;
}

/* READ FUNCTION */
static ssize_t read_dev(struct file *f, char __user *buf,
                        size_t len, loff_t *off)
{
    char tmp[32];
    int n;
    ktime_t start, end;
    u32 distance_cm;

    if (*off > 0)
        return 0;

    /* Trigger pulse */
    gpio_clear(TRIG);
    udelay(2);
    gpio_set(TRIG);
    udelay(10);
    gpio_clear(TRIG);

    /* Wait for echo HIGH */
    while (!gpio_read(ECHO));

    start = ktime_get();

    /* Wait for echo LOW */
    while (gpio_read(ECHO));

    end = ktime_get();

    s64 duration = ktime_to_ns(ktime_sub(end, start));
    u32 time_us = duration / 1000;

    distance_cm = (time_us * 34) / 2000;

    n = sprintf(tmp, "%u cm\n", distance_cm);

    if (copy_to_user(buf, tmp, n))
        return -EFAULT;

    *off += n;
    return n;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = read_dev,
};

/* INIT */
static int __init us_init(void)
{
    pr_info("Driver init start\n");

    gpio_base = ioremap(GPIO_BASE, 0xB4);
    if (!gpio_base)
        return -ENOMEM;

    gpio_set_output(TRIG);
    gpio_set_input(ECHO);

    major = register_chrdev(0, DEVICE, &fops);

    pr_info("Major number = %d\n", major);
    pr_info("Driver loaded successfully\n");

    return 0;
}

/* EXIT */
static void __exit us_exit(void)
{
    iounmap(gpio_base);
    unregister_chrdev(major, DEVICE);

    pr_info("Driver removed\n");
}

module_init(us_init);
module_exit(us_exit);

MODULE_LICENSE("GPL");
