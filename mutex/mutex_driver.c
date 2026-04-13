// SPDX-License-Identifier: GPL
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/mutex.h>

#define DEVICE_NAME "multi_user_dev"
#define CLASS_NAME  "multi_class"

static int major;
static struct class *dev_class;
static struct device *dev_device;

static DEFINE_MUTEX(my_mutex);     // 🔐 Critical section lock
static int shared_counter = 0;     // Shared data

/* Open */
static int dev_open(struct inode *inode, struct file *file)
{
    pr_info("Device opened\n");
    return 0;
}

/* Read */
static ssize_t dev_read(struct file *file, char __user *buf,
                        size_t len, loff_t *offset)
{
    char message[50];
    int msg_len;

    mutex_lock(&my_mutex);   // 🔐 lock start

    msg_len = sprintf(message, "Counter value: %d\n", shared_counter);

    mutex_unlock(&my_mutex); // 🔐 lock end

    return simple_read_from_buffer(buf, len, offset,
                                   message, msg_len);
}

/* Write */
static ssize_t dev_write(struct file *file, const char __user *buf,
                         size_t len, loff_t *offset)
{
    mutex_lock(&my_mutex);   // 🔐 Critical Section

    shared_counter++;        // Only one user modifies at a time
    pr_info("Counter incremented: %d\n", shared_counter);

    mutex_unlock(&my_mutex);

    return len;
}

/* Release */
static int dev_release(struct inode *inode, struct file *file)
{
    pr_info("Device closed\n");
    return 0;
}

static struct file_operations fops = {
    .owner   = THIS_MODULE,
    .open    = dev_open,
    .read    = dev_read,
    .write   = dev_write,
    .release = dev_release,
};

/* Init */
static int __init driver_init(void)
{
    major = register_chrdev(0, DEVICE_NAME, &fops);

    dev_class = class_create(THIS_MODULE, CLASS_NAME);
    dev_device = device_create(dev_class, NULL,
                               MKDEV(major, 0), NULL,
                               DEVICE_NAME);

    mutex_init(&my_mutex);

    pr_info("Multi-user driver loaded\n");
    return 0;
}

/* Exit */
static void __exit driver_exit(void)
{
    device_destroy(dev_class, MKDEV(major, 0));
    class_destroy(dev_class);
    unregister_chrdev(major, DEVICE_NAME);

    pr_info("Driver removed\n");
}

module_init(driver_init);
module_exit(driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Sharang");
MODULE_DESCRIPTION("Multi-user safe kernel driver");
