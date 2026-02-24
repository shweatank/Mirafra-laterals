#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "adddriver"

static int major;
static int sum = 0;           
static char msg[100];         

/* Open */
static int my_open(struct inode *inode, struct file *file) {
    printk(KERN_INFO "adddriver: device opened\n");
    return 0;
}


static int my_release(struct inode *inode, struct file *file) {
    printk(KERN_INFO "adddriver: device closed\n");
    return 0;
}


static ssize_t my_write(struct file *file, const char __user *user_buffer,
                        size_t count, loff_t *offset)
{
    char buf[50];
    int n1, n2;

    if (count >= sizeof(buf))
        return -EINVAL;

    if (copy_from_user(buf, user_buffer, count))
        return -EFAULT;

    buf[count] = '\0';

    // parse two integers from string
    if (sscanf(buf, "%d %d", &n1, &n2) != 2)
        return -EINVAL;

    sum = n1 + n2;  // kernel-space addition
    printk(KERN_INFO "adddriver: %d + %d = %d\n", n1, n2, sum);

    return count;
}


static ssize_t my_read(struct file *file, char __user *user_buffer,
                       size_t count, loff_t *offset)
{
    int len;
    if (*offset > 0) 
        return 0;

    len = snprintf(msg, sizeof(msg), "Sum = %d\n", sum);

    if (copy_to_user(user_buffer, msg, len))
        return -EFAULT;

    *offset += len;
    return len;
}

/* File operations */
static struct file_operations fops = {
    .owner   = THIS_MODULE,
    .open    = my_open,
    .release = my_release,
    .read    = my_read,
    .write   = my_write,
};

/* Module init */
static int __init add_init(void) {
    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0) {
        printk(KERN_ALERT "adddriver failed\n");
        return major;
    }
    printk(KERN_INFO "adddriver loaded with major %d\n", major);
    return 0;
}

/* Module exit */
static void __exit add_exit(void) {
    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO "adddriver unloaded\n");
}

module_init(add_init);
module_exit(add_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Isshika");
MODULE_DESCRIPTION("Simple add driver");
