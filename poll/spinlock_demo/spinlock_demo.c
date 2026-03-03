#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/spinlock.h>

#define DEVICE_NAME "spin_demo"

static dev_t dev;
static struct cdev cdev_obj;

static int shared_counter = 0;
static spinlock_t my_spinlock;

/* ---------- WRITE ---------- */
static ssize_t my_write(struct file *file,
                        const char __user *buf,
                        size_t len,
                        loff_t *off)
{
    unsigned long flags;

    spin_lock_irqsave(&my_spinlock, flags);

    pr_info("Process %d entered critical section\n",
            current->pid);

    shared_counter++;
    pr_info("Counter = %d\n", shared_counter);

    spin_unlock_irqrestore(&my_spinlock, flags);

    return len;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .write = my_write,
};

static int __init my_init(void)
{
    alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME);
    cdev_init(&cdev_obj, &fops);
    cdev_add(&cdev_obj, dev, 1);

    spin_lock_init(&my_spinlock);

    pr_info("Spinlock Demo Driver Loaded\n");
    return 0;
}

static void __exit my_exit(void)
{
    cdev_del(&cdev_obj);
    unregister_chrdev_region(dev, 1);

    pr_info("Spinlock Demo Driver Unloaded\n");
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
