#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/timekeeping.h>
#include <linux/delay.h>
#include <linux/io.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/kthread.h>

#define DEVICE "us_dev"

/* BCM2711 base addresses */
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

/* Wait queue for blocking read */
static DECLARE_WAIT_QUEUE_HEAD(distance_wq);
static bool distance_updated = false;
static u32 last_distance = 0;

/* GPIO helpers */
static void gpio_set_output(int pin) {
    u32 reg = (pin < 10) ? GPFSEL0 : (pin < 20) ? GPFSEL1 : GPFSEL2;
    u32 shift = (pin % 10) * 3;
    u32 val = ioread32(gpio_base + reg);
    val &= ~(7 << shift);
    val |= (1 << shift);
    iowrite32(val, gpio_base + reg);
}

static void gpio_set_input(int pin) {
    u32 reg = (pin < 10) ? GPFSEL0 : (pin < 20) ? GPFSEL1 : GPFSEL2;
    u32 shift = (pin % 10) * 3;
    u32 val = ioread32(gpio_base + reg);
    val &= ~(7 << shift);
    iowrite32(val, gpio_base + reg);
}

static void gpio_set(int pin) {
    iowrite32(1 << pin, gpio_base + GPSET0);
}

static void gpio_clear(int pin) {
    iowrite32(1 << pin, gpio_base + GPCLR0);
}

static int gpio_read(int pin) {
    return (ioread32(gpio_base + GPLEV0) & (1 << pin)) ? 1 : 0;
}

/* Measure distance */
static u32 measure_distance(void) {
    ktime_t start, end;
    s64 duration;
    u32 time_us, distance_cm;

    gpio_clear(TRIG);
    udelay(2);
    gpio_set(TRIG);
    udelay(10);
    gpio_clear(TRIG);

    while (!gpio_read(ECHO)); // wait HIGH
    start = ktime_get();

    while (gpio_read(ECHO));  // wait LOW
    end = ktime_get();

    duration = ktime_to_ns(ktime_sub(end, start));
    time_us = duration / 1000;
    distance_cm = (time_us * 34) / 2000;

    return distance_cm;
}

/* READ FUNCTION */
static ssize_t read_dev(struct file *f, char __user *buf,
                        size_t len, loff_t *off)
{
    char tmp[32];
    int n;
    u32 dist;

    /* Block until distance changes */
    wait_event_interruptible(distance_wq, distance_updated);

    dist = last_distance;
    distance_updated = false;  // reset for next change

    n = snprintf(tmp, sizeof(tmp), "%u cm\n", dist);
    if (copy_to_user(buf, tmp, n))
        return -EFAULT;

    *off += n;
    return n;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = read_dev,
};

/* Thread to continuously monitor distance */
static struct task_struct *distance_task;
static int distance_thread(void *data)
{
    while (!kthread_should_stop()) {
        u32 dist = measure_distance();
        if (dist != last_distance) {
            last_distance = dist;
            distance_updated = true;
            wake_up_interruptible(&distance_wq);
        }
        msleep(100);  // adjust measurement interval
    }
    return 0;
}

/* INIT */
static int __init us_init(void)
{
    pr_info("Ultrasonic driver init\n");

    gpio_base = ioremap(GPIO_BASE, 0xB4);
    if (!gpio_base)
        return -ENOMEM;

    gpio_set_output(TRIG);
    gpio_set_input(ECHO);

    major = register_chrdev(0, DEVICE, &fops);
    pr_info("Major number = %d\n", major);

    distance_task = kthread_run(distance_thread, NULL, "dist_monitor");
    if (IS_ERR(distance_task))
        return PTR_ERR(distance_task);

    pr_info("Driver loaded successfully\n");
    return 0;
}

/* EXIT */
static void __exit us_exit(void)
{
    if (distance_task)
        kthread_stop(distance_task);

    iounmap(gpio_base);
    unregister_chrdev(major, DEVICE);
    pr_info("Driver removed\n");
}

module_init(us_init);
module_exit(us_exit);
MODULE_LICENSE("GPL");
