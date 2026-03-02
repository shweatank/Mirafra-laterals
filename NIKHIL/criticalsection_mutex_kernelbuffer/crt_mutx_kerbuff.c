#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/minmax.h>
#include <linux/mutex.h>

#define DEVICE_NAME "crt_mutx_kerbuff"
#define BUFFER_SIZE 256

static int major_number;
static char kernel_buffer[BUFFER_SIZE];
static int buffer_size = 0;

/* 🔐 Mutex for protecting critical section */
static DEFINE_MUTEX(buffer_mutex);

/* Called when device is opened */
static int basic_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "basic_char: device opened\n");
    return 0;
}

/* Called when device is closed */
static int basic_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "basic_char: device closed\n");
    return 0;
}

/* Read function (Protected Critical Section) */
static ssize_t basic_read(struct file *file,
                          char __user *user_buffer,
                          size_t count,
                          loff_t *offset)
{
    int bytes_to_copy;

    mutex_lock(&buffer_mutex);   // 🔐 Lock

    if (*offset >= buffer_size) {
        mutex_unlock(&buffer_mutex);
        return 0;   // EOF
    }

    bytes_to_copy = min(count, (size_t)(buffer_size - *offset));

    if (copy_to_user(user_buffer,
                     kernel_buffer + *offset,
                     bytes_to_copy)) {
        mutex_unlock(&buffer_mutex);
        return -EFAULT;
    }

    *offset += bytes_to_copy;

    mutex_unlock(&buffer_mutex); // 🔓 Unlock

    printk(KERN_INFO "basic_char: read %d bytes\n", bytes_to_copy);

    return bytes_to_copy;
}

/* Write function (Protected Critical Section) */
static ssize_t basic_write(struct file *file,
                           const char __user *user_buffer,
                           size_t count,
                           loff_t *offset)
{
    int bytes_to_copy;

    bytes_to_copy = min(count, (size_t)BUFFER_SIZE);

    mutex_lock(&buffer_mutex);   // 🔐 Lock

    if (copy_from_user(kernel_buffer,
                       user_buffer,
                       bytes_to_copy)) {
        mutex_unlock(&buffer_mutex);
        return -EFAULT;
    }

    buffer_size = bytes_to_copy;

    mutex_unlock(&buffer_mutex); // 🔓 Unlock

    printk(KERN_INFO "basic_char: written %d bytes\n", bytes_to_copy);

    return bytes_to_copy;
}

/* File operations */
static struct file_operations basic_fops = {
    .owner   = THIS_MODULE,
    .open    = basic_open,
    .read    = basic_read,
    .write   = basic_write,
    .release = basic_release,
};

/* Module Init */
static int __init basic_char_init(void)
{
    major_number = register_chrdev(0, DEVICE_NAME, &basic_fops);

    if (major_number < 0) {
        printk(KERN_ALERT "basic_char: failed to register device\n");
        return major_number;
    }

    printk(KERN_INFO "basic_char: loaded successfully\n");
    printk(KERN_INFO "Major number = %d\n", major_number);

    return 0;
}

/* Module Exit */
static void __exit basic_char_exit(void)
{
    unregister_chrdev(major_number, DEVICE_NAME);
    printk(KERN_INFO "basic_char: unloaded\n");
}

module_init(basic_char_init);
module_exit(basic_char_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Nikhil");
MODULE_DESCRIPTION("Mutex protected character driver");