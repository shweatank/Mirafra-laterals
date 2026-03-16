// timer_driver.c
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/timer.h>
#include <linux/device.h>

#define DEVICE_NAME "timer_iss"

static struct timer_list my_timer;
static int event_flag = 0;
static int major;
static struct class *cls;

static void timer_callback(struct timer_list *t)
{
    event_flag = 1;
    printk("Timer interrupt triggered\n");

    mod_timer(&my_timer, jiffies + msecs_to_jiffies(2000));
}

static ssize_t dev_read(struct file *f, char __user *buf,
                        size_t len, loff_t *off)
{
    int val;

    if(!event_flag)
        return 0;

    val = 1;
    event_flag = 0;

    copy_to_user(buf, &val, sizeof(val));

    return sizeof(val);
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = dev_read,
};

static int __init timer_init(void)
{
    major = register_chrdev(0, DEVICE_NAME, &fops);

    cls = class_create(DEVICE_NAME);
    device_create(cls, NULL, MKDEV(major,0), NULL, DEVICE_NAME);

    timer_setup(&my_timer, timer_callback, 0);
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(2000));

    printk("Timer driver loaded\n");
    return 0;
}

static void __exit timer_exit(void)
{
    del_timer(&my_timer);

    device_destroy(cls, MKDEV(major,0));
    class_destroy(cls);
    unregister_chrdev(major, DEVICE_NAME);

    printk("Timer driver removed\n");
}

module_init(timer_init);
module_exit(timer_exit);

MODULE_LICENSE("GPL");
