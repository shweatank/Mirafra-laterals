#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/string.h>

#define DEVICE_NAME "devcal"
#define CLASS_NAME  "devcalclass"

static dev_t dev_num;
static struct cdev calc_cdev;
static struct class *calc_class;

static char result[100];   // buffer returned to user

static int calc_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "calcdev opened\n");
    return 0;
}

static int calc_close(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "calcdev closed\n");
    return 0;
}

/* WRITE: receive & compute */
static ssize_t calc_write(struct file *file,
                          const char __user *buf,
                          size_t len, loff_t *off)
{
    char kbuf[100];
    int a, b;
    char op[10];
    long res;

    if (len > sizeof(kbuf) - 1)
        len = sizeof(kbuf) - 1;

    if (copy_from_user(kbuf, buf, len))
        return -EFAULT;

    kbuf[len] = '\0';

    printk(KERN_INFO "Received: %s\n", kbuf);

    /* Parse input */
    if (sscanf(kbuf, "%d %d %s", &a, &b, op) != 3) {
        snprintf(result, sizeof(result), "Invalid input\n");
        return len;
    }

    /* Perform operation */
    if (strcmp(op, "add") == 0)
        res = a + b;
    else if (strcmp(op, "sub") == 0)
        res = a - b;
    else if (strcmp(op, "mul") == 0)
        res = a * b;
    else if (strcmp(op, "div") == 0) {
        if (b == 0) {
            snprintf(result, sizeof(result),
                     "Error: divide by zero\n");
            return len;
        }
        res = a / b;
    } else {
        snprintf(result, sizeof(result),
                 "Unknown operation\n");
        return len;
    }

    snprintf(result, sizeof(result),
             "Result = %ld\n", res);

    return len;
}

/* READ: send result */
static ssize_t calc_read(struct file *file,
                         char __user *buf,
                         size_t len, loff_t *off)
{
    return simple_read_from_buffer(buf, len, off,
                                   result, strlen(result));
}

static struct file_operations fops =
{
    .owner   = THIS_MODULE,
    .open    = calc_open,
    .release = calc_close,
    .write   = calc_write,
    .read    = calc_read,
};

static int __init calc_init(void)
{
    alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);

    cdev_init(&calc_cdev, &fops);
    cdev_add(&calc_cdev, dev_num, 1);

    calc_class = class_create(CLASS_NAME);
    device_create(calc_class, NULL, dev_num, NULL, DEVICE_NAME);

    printk(KERN_INFO "Calculator driver loaded\n");
    return 0;
}

static void __exit calc_exit(void)
{
    device_destroy(calc_class, dev_num);
    class_destroy(calc_class);
    cdev_del(&calc_cdev);
    unregister_chrdev_region(dev_num, 1);

    printk(KERN_INFO "Calculator driver unloaded\n");
}

module_init(calc_init);
module_exit(calc_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("You");
MODULE_DESCRIPTION("Calculator driver with parsing in write()");
