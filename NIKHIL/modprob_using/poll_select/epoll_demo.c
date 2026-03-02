// epoll_select_demo.c
// Minimal char driver that supports select()/poll()/epoll() via .poll
// Demonstrates BOTH mutex and spinlock in a simple, working way.
//
// - Write to /dev/epoll_demo -> data goes into a kfifo (protected by spinlock)
// - Read from /dev/epoll_demo -> blocks until data available (waitqueue + poll)
// - ioctl to set a dummy "mode" protected by mutex
//
// Tested design for Linux 5.x/6.x kernels.

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include <linux/poll.h>
#include <linux/wait.h>
#include <linux/kfifo.h>
#include <linux/mutex.h>
#include <linux/spinlock.h>
#include <linux/ioctl.h>

#define DRV_NAME        "epoll_demo"
#define DEV_NODE_NAME   "epoll_demo"
#define FIFO_SIZE       4096

// Simple ioctl interface
#define EPOLLDEMO_IOC_MAGIC   'E'
#define EPOLLDEMO_IOC_SETMODE _IOW(EPOLLDEMO_IOC_MAGIC, 1, int)

struct epoll_demo_dev {
    dev_t devt;
    struct cdev cdev;
    struct class *class;
    struct device *device;

    // Data path
    struct kfifo fifo;
    spinlock_t fifo_lock;      // protects kfifo access
    wait_queue_head_t rd_wq;   // readers wait here for data

    // Control path
    struct mutex cfg_lock;     // protects config changes
    int mode;
};

static struct epoll_demo_dev gdev;

static int epoll_demo_open(struct inode *inode, struct file *filp)
{
    filp->private_data = &gdev;
    return 0;
}

static int epoll_demo_release(struct inode *inode, struct file *filp)
{
    return 0;
}

static ssize_t epoll_demo_write(struct file *filp,
                                const char __user *ubuf,
                                size_t len,
                                loff_t *off)
{
    struct epoll_demo_dev *d = filp->private_data;
    unsigned int copied = 0;
    int ret;
    unsigned long flags;

    if (len == 0)
        return 0;

    // kfifo_from_user already handles copy_from_user safely.
    // We still protect the fifo with spinlock because this is the fast path.
    spin_lock_irqsave(&d->fifo_lock, flags);
    ret = kfifo_from_user(&d->fifo, ubuf, len, &copied);
    spin_unlock_irqrestore(&d->fifo_lock, flags);

    if (ret)
        return ret;

    if (copied > 0) {
        // Wake up readers waiting in read() or poll()/epoll()/select()
        wake_up_interruptible(&d->rd_wq);
    }

    return copied;
}

static ssize_t epoll_demo_read(struct file *filp,
                               char __user *ubuf,
                               size_t len,
                               loff_t *off)
{
    struct epoll_demo_dev *d = filp->private_data;
    unsigned int copied = 0;
    int ret;
    unsigned long flags;

    if (len == 0)
        return 0;

    // Block until data exists (unless O_NONBLOCK)
    if (kfifo_is_empty(&d->fifo)) {
        if (filp->f_flags & O_NONBLOCK)
            return -EAGAIN;

        ret = wait_event_interruptible(d->rd_wq, !kfifo_is_empty(&d->fifo));
        if (ret)
            return ret;
    }

    // Pop from fifo (protected by spinlock)
    spin_lock_irqsave(&d->fifo_lock, flags);
    ret = kfifo_to_user(&d->fifo, ubuf, len, &copied);
    spin_unlock_irqrestore(&d->fifo_lock, flags);

    if (ret)
        return ret;

    return copied;
}

// The key for select()/poll()/epoll()
static __poll_t epoll_demo_poll(struct file *filp, poll_table *wait)
{
    struct epoll_demo_dev *d = filp->private_data;
    __poll_t mask = 0;
    unsigned long flags;

    // Tell the kernel which waitqueue to attach this file to
    poll_wait(filp, &d->rd_wq, wait);

    // Check readiness
    spin_lock_irqsave(&d->fifo_lock, flags);
    if (!kfifo_is_empty(&d->fifo))
        mask |= (POLLIN | POLLRDNORM);
    if (!kfifo_is_full(&d->fifo))
        mask |= (POLLOUT | POLLWRNORM);
    spin_unlock_irqrestore(&d->fifo_lock, flags);

    return mask;
}

static long epoll_demo_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    struct epoll_demo_dev *d = filp->private_data;
    int mode;

    switch (cmd) {
    case EPOLLDEMO_IOC_SETMODE:
        if (copy_from_user(&mode, (void __user *)arg, sizeof(mode)))
            return -EFAULT;

        // Slow/control path uses mutex (may sleep)
        mutex_lock(&d->cfg_lock);
        d->mode = mode;
        mutex_unlock(&d->cfg_lock);

        pr_info(DRV_NAME ": mode set to %d\n", mode);
        return 0;

    default:
        return -ENOTTY;
    }
}

static const struct file_operations epoll_demo_fops = {
    .owner          = THIS_MODULE,
    .open           = epoll_demo_open,
    .release        = epoll_demo_release,
    .read           = epoll_demo_read,
    .write          = epoll_demo_write,
    .poll           = epoll_demo_poll,
    .unlocked_ioctl = epoll_demo_ioctl,
    .llseek         = no_llseek,
};

static int __init epoll_demo_init(void)
{
    int ret;

    spin_lock_init(&gdev.fifo_lock);
    mutex_init(&gdev.cfg_lock);
    init_waitqueue_head(&gdev.rd_wq);
    gdev.mode = 0;

    ret = kfifo_alloc(&gdev.fifo, FIFO_SIZE, GFP_KERNEL);
    if (ret) {
        pr_err(DRV_NAME ": kfifo_alloc failed: %d\n", ret);
        return ret;
    }

    ret = alloc_chrdev_region(&gdev.devt, 0, 1, DRV_NAME);
    if (ret) {
        pr_err(DRV_NAME ": alloc_chrdev_region failed: %d\n", ret);
        goto err_fifo;
    }

    cdev_init(&gdev.cdev, &epoll_demo_fops);
    gdev.cdev.owner = THIS_MODULE;

    ret = cdev_add(&gdev.cdev, gdev.devt, 1);
    if (ret) {
        pr_err(DRV_NAME ": cdev_add failed: %d\n", ret);
        goto err_chrdev;
    }

    gdev.class = class_create(DRV_NAME);
    if (IS_ERR(gdev.class)) {
        ret = PTR_ERR(gdev.class);
        pr_err(DRV_NAME ": class_create failed: %d\n", ret);
        goto err_cdev;
    }

    gdev.device = device_create(gdev.class, NULL, gdev.devt, NULL, DEV_NODE_NAME);
    if (IS_ERR(gdev.device)) {
        ret = PTR_ERR(gdev.device);
        pr_err(DRV_NAME ": device_create failed: %d\n", ret);
        goto err_class;
    }

    pr_info(DRV_NAME ": loaded. /dev/%s major=%d minor=%d\n",
            DEV_NODE_NAME, MAJOR(gdev.devt), MINOR(gdev.devt));
    return 0;

err_class:
    class_destroy(gdev.class);
err_cdev:
    cdev_del(&gdev.cdev);
err_chrdev:
    unregister_chrdev_region(gdev.devt, 1);
err_fifo:
    kfifo_free(&gdev.fifo);
    return ret;
}

static void __exit epoll_demo_exit(void)
{
    device_destroy(gdev.class, gdev.devt);
    class_destroy(gdev.class);
    cdev_del(&gdev.cdev);
    unregister_chrdev_region(gdev.devt, 1);
    kfifo_free(&gdev.fifo);
    pr_info(DRV_NAME ": unloaded\n");
}

module_init(epoll_demo_init);
module_exit(epoll_demo_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("techdhaba");
MODULE_DESCRIPTION("Char driver demo: poll/select/epoll + mutex + spinlock");