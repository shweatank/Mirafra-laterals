#include <linux/module.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/mutex.h>
#include <linux/delay.h>

#define DEVICE_NAME "mutex_demo"

static dev_t dev;
static struct cdev cdev_obj;

static int shared_counter = 0;
static DEFINE_MUTEX(my_mutex);

/* ---------- WRITE ---------- */
static ssize_t my_write(struct file *file,
                        const char __user *buf,
                        size_t len,
                        loff_t *off)
{
    mutex_lock(&my_mutex);

    pr_info("Process %d entered critical section\n",
            current->pid);

    shared_counter++;
    pr_info("Counter = %d\n", shared_counter);

    msleep(3000);   // simulate long operation

    pr_info("Process %d leaving critical section\n",
            current->pid);

    mutex_unlock(&my_mutex);

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

    pr_info("Mutex Demo Driver Loaded\n");
    return 0;
}

static void __exit my_exit(void)
{
    cdev_del(&cdev_obj);
    unregister_chrdev_region(dev, 1);

    pr_info("Mutex Demo Driver Unloaded\n");
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");
