#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/poll.h>
#include <linux/wait.h>
#include <linux/mutex.h>
#include <linux/spinlock.h>

#define DEVICE_NAME "epoll_demo"
#define BUF_SIZE 64

static dev_t devno;
static struct cdev demo_cdev;

static struct class *dev_class;

static char buffer[BUF_SIZE];
static int data_available;

static DEFINE_MUTEX(demo_mutex);        /* Sleepable lock */
static DEFINE_SPINLOCK(demo_spinlock);  /* Fast lock */

static DECLARE_WAIT_QUEUE_HEAD(demo_wq);

/* ---------------- Read ---------------- */
static ssize_t demo_read(struct file *file,
                         char __user *ubuf,
                         size_t count,
                         loff_t *ppos)
{
    ssize_t ret;

    if (mutex_lock_interruptible(&demo_mutex))
        return -ERESTARTSYS;

    if (!data_available) {
        mutex_unlock(&demo_mutex);
        return 0;
    }

    count = min(count, (size_t)BUF_SIZE);

    if (copy_to_user(ubuf, buffer, count)) {
        mutex_unlock(&demo_mutex);
        return -EFAULT;
    }

    spin_lock(&demo_spinlock);
    data_available = 0;
    spin_unlock(&demo_spinlock);

    ret = count;
    mutex_unlock(&demo_mutex);
    return ret;
}

/* ---------------- Write ---------------- */
static ssize_t demo_write(struct file *file,
                          const char __user *ubuf,
                          size_t count,
                          loff_t *ppos)
{
    if (mutex_lock_interruptible(&demo_mutex))
        return -ERESTARTSYS;

    count = min(count, (size_t)BUF_SIZE);

    if (copy_from_user(buffer, ubuf, count)) {
        mutex_unlock(&demo_mutex);
        return -EFAULT;
    }

    spin_lock(&demo_spinlock);
    data_available = 1;
    spin_unlock(&demo_spinlock);

    mutex_unlock(&demo_mutex);

    /* Wake select/epoll */
    wake_up_interruptible(&demo_wq);

    return count;
}

/* ---------------- poll() ---------------- */
static __poll_t demo_poll(struct file *file, poll_table *wait)
{
    __poll_t mask = 0;

    poll_wait(file, &demo_wq, wait);

    spin_lock(&demo_spinlock);
    if (data_available)
        mask |= POLLIN | POLLRDNORM;
    spin_unlock(&demo_spinlock);

    return mask;
}

/* ---------------- File Ops ---------------- */
static const struct file_operations demo_fops = {
    .owner = THIS_MODULE,
    .read  = demo_read,
    .write = demo_write,
    .poll  = demo_poll,
};

/* ---------------- Init / Exit ---------------- */
static int __init demo_init(void)
{
    alloc_chrdev_region(&devno, 0, 1, DEVICE_NAME);
    cdev_init(&demo_cdev, &demo_fops);
    cdev_add(&demo_cdev, devno, 1);

    dev_class = class_create("Class");
    device_create(dev_class,NULL,devno,NULL,DEVICE_NAME);

    pr_info("epoll_demo loaded: major=%d\n", MAJOR(devno));
    return 0;
}

static void __exit demo_exit(void)
{
    device_destroy(dev_class,devno);
    class_destroy(dev_class);
    cdev_del(&demo_cdev);

    unregister_chrdev_region(devno, 1);
    pr_info("epoll_demo unloaded\n");
}

module_init(demo_init);
module_exit(demo_exit);

MODULE_LICENSE("GPL");