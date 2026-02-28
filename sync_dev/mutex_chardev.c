// SPDX-License-Identifier: GPL
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/mutex.h>

#define DRIVER_NAME "mutex_chardev"
#define BUF_SIZE     1024

static int major;
static char kernel_buf[BUF_SIZE];
static size_t buf_len = 0;

static DEFINE_MUTEX(my_mutex);

static int my_open(struct inode *inode, struct file *file)
{
    pr_info(DRIVER_NAME" Device opened by PID %d\n", current->pid);
    return 0;
}

static int my_release(struct inode *inode, struct file *file)
{
    pr_info(DRIVER_NAME" Device released by PID %d\n", current->pid);
    return 0;
}

static ssize_t my_read(struct file *file, char __user *ubuf,
                       size_t count, loff_t *ppos)
{
    int ret = 0;
    size_t to_copy;

    if (*ppos >= buf_len)
        return 0;

    to_copy = min(count, buf_len - (size_t)*ppos);

    mutex_lock(&my_mutex);

    if (copy_to_user(ubuf, kernel_buf + *ppos, to_copy)) {
        ret = -EFAULT;
        goto out;
    }

    *ppos += to_copy;
    pr_info(DRIVER_NAME" Read %zu bytes\n", to_copy);
    ret = to_copy;

out:
    mutex_unlock(&my_mutex);
    return ret;
}

static ssize_t my_write(struct file *file, const char __user *ubuf,
                        size_t count, loff_t *ppos)
{
    int ret = 0;
    size_t to_copy = min(count, (size_t)(BUF_SIZE - 1));

    mutex_lock(&my_mutex);

    if (copy_from_user(kernel_buf, ubuf, to_copy)) {
        ret = -EFAULT;
        goto out;
    }

    kernel_buf[to_copy] = '\0';
    buf_len = to_copy;
    pr_info(DRIVER_NAME" Written %zu bytes: %s\n", to_copy, kernel_buf);
    ret = to_copy;

out:
    mutex_unlock(&my_mutex);
    return ret;
}

static const struct file_operations my_fops = {
    .owner   = THIS_MODULE,
    .open    = my_open,
    .release = my_release,
    .read    = my_read,
    .write   = my_write,
};

static int __init my_init(void)
{
    major = register_chrdev(0, DRIVER_NAME, &my_fops);
    if (major < 0) {
        pr_err(DRIVER_NAME" Failed to register\n");
        return major;
    }
    pr_info(DRIVER_NAME" Registered with major: %d\n", major);
    return 0;
}

static void __exit my_exit(void)
{
    unregister_chrdev(major, DRIVER_NAME);
    pr_info(DRIVER_NAME" Unregistered\n");
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Adepu Shashank");
MODULE_DESCRIPTION("Single access character driver using mutex");