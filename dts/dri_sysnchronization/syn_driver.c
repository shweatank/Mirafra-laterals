#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/mutex.h>
#include <linux/delay.h>
#include <linux/sched.h>

#define DRV_NAME     "cri_drv"
#define DEVICE_NAME  "critical_driver"
#define BUF_SIZE     1024

static dev_t dev;
static struct cdev c_dev;
static struct class *dev_class;

/* Shared Critical Buffer */
static char kbuffer[BUF_SIZE];
static size_t data_size = 0;

/* Mutex protecting shared critical section */
static DEFINE_MUTEX(cri_mutex);

/* ---------- File Operations ---------- */

static int cri_open(struct inode *inode, struct file *file)
{
    pr_info("[CRI_DRV] Device opened by PID %d\n", current->pid);
    return 0;
}

static int cri_release(struct inode *inode, struct file *file)
{
    pr_info("[CRI_DRV] Device closed by PID %d\n", current->pid);
    return 0;
}

/* -------- Protected READ -------- */

static ssize_t cri_read(struct file *file, char __user *buf,
                        size_t count, loff_t *ppos)
{
    ssize_t ret;

    pr_info("[READ] PID %d trying to enter critical section\n", current->pid);

    if (!mutex_trylock(&cri_mutex)) {
        pr_info("[READ] PID %d waiting for mutex...\n", current->pid);
        mutex_lock(&cri_mutex);
    }

    pr_info("[READ] PID %d acquired mutex\n", current->pid);

    if (*ppos >= data_size) {
        pr_info("[READ] PID %d EOF reached\n", current->pid);
        mutex_unlock(&cri_mutex);
        return 0;
    }

    if (count > data_size - *ppos)
        count = data_size - *ppos;

    if (copy_to_user(buf, kbuffer + *ppos, count)) {
        pr_err("[READ] copy_to_user failed\n");
        mutex_unlock(&cri_mutex);
        return -EFAULT;
    }

    *ppos += count;
    ret = count;

    /* Simulate long critical section */
    msleep(3000);

    pr_info("[READ] PID %d releasing mutex\n", current->pid);
    mutex_unlock(&cri_mutex);

    return ret;
}

/* -------- Protected WRITE -------- */

static ssize_t cri_write(struct file *file, const char __user *buf,
                         size_t count, loff_t *ppos)
{
    ssize_t ret;

    pr_info("[WRITE] PID %d trying to enter critical section\n", current->pid);

    if (!mutex_trylock(&cri_mutex)) {
        pr_info("[WRITE] PID %d waiting for mutex...\n", current->pid);
        mutex_lock(&cri_mutex);
    }

    pr_info("[WRITE] PID %d acquired mutex\n", current->pid);

    if (count > BUF_SIZE)
        count = BUF_SIZE;

    memset(kbuffer, 0, BUF_SIZE);

    if (copy_from_user(kbuffer, buf, count)) {
        pr_err("[WRITE] copy_from_user failed\n");
        mutex_unlock(&cri_mutex);
        return -EFAULT;
    }

    data_size = count;
    ret = count;

    /* Simulate long critical section */
    msleep(3000);

    pr_info("[WRITE] PID %d releasing mutex\n", current->pid);
    mutex_unlock(&cri_mutex);

    return ret;
}

static struct file_operations fops = {
    .owner   = THIS_MODULE,
    .open    = cri_open,
    .release = cri_release,
    .read    = cri_read,
    .write   = cri_write,
};

/* ---------- Module Init ---------- */

static int __init cri_driver_init(void)
{
    int ret;

    pr_info("[CRI_DRV] Initializing driver...\n");

    ret = alloc_chrdev_region(&dev, 0, 1, DRV_NAME);
    if (ret < 0) {
        pr_err("[CRI_DRV] Failed to allocate device number\n");
        return ret;
    }

    pr_info("[CRI_DRV] Major=%d Minor=%d\n",
            MAJOR(dev), MINOR(dev));

    cdev_init(&c_dev, &fops);

    ret = cdev_add(&c_dev, dev, 1);
    if (ret < 0) {
        unregister_chrdev_region(dev, 1);
        return ret;
    }

    dev_class = class_create("dev_class");
    if (IS_ERR(dev_class)) {
        cdev_del(&c_dev);
        unregister_chrdev_region(dev, 1);
        return PTR_ERR(dev_class);
    }

    device_create(dev_class, NULL, dev, NULL, DEVICE_NAME);

    pr_info("[CRI_DRV] Device created: /dev/%s\n", DEVICE_NAME);
    pr_info("[CRI_DRV] Driver loaded successfully\n");

    return 0;
}

/* ---------- Module Exit ---------- */

static void __exit cri_driver_exit(void)
{
    pr_info("[CRI_DRV] Removing driver...\n");

    device_destroy(dev_class, dev);
    class_destroy(dev_class);
    cdev_del(&c_dev);
    unregister_chrdev_region(dev, 1);

    pr_info("[CRI_DRV] Driver removed successfully\n");
}

module_init(cri_driver_init);
module_exit(cri_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Narsimha");
MODULE_DESCRIPTION("Character Driver Demonstrating Shared Critical Section with Mutex");