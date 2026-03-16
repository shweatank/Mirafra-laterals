// timer_driver.c

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/timer.h>
#include <linux/device.h>
#include <linux/cdev.h>

#define DEVICE_NAME "timer_device"
#define CLASS_NAME  "timer_class"

static struct timer_list my_timer;
static int flag = 0;

/* character device structures */
static dev_t dev_num;
static struct cdev timer_cdev;
static struct class *timer_class;

/* -------------------------------------------------- */
/* Timer Callback                                     */
/* -------------------------------------------------- */

static void timer_callback(struct timer_list *t)
{
    flag = 1;

    pr_info("Timer interrupt triggered\n");

    /* restart timer after 5 seconds */
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(5000));
}

/* -------------------------------------------------- */
/* Device Read                                        */
/* -------------------------------------------------- */

static ssize_t dev_read(struct file *file,
                        char __user *buf,
                        size_t len,
                        loff_t *offset)
{
    
   

    if (!flag)
        return 0;

    flag = 0;
    return 1;
}

/* -------------------------------------------------- */
/* File Operations                                    */
/* -------------------------------------------------- */

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read  = dev_read,
};

/* -------------------------------------------------- */
/* Module Init                                        */
/* -------------------------------------------------- */

static int __init timer_init(void)
{
    int ret;

    /* allocate device number */
    ret = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
    if (ret)
        return ret;

    /* initialize cdev */
    cdev_init(&timer_cdev, &fops);

    ret = cdev_add(&timer_cdev, dev_num, 1);
    if (ret)
        goto unregister;

    /* create class */
    timer_class = class_create(CLASS_NAME);
    if (IS_ERR(timer_class)) {
        ret = PTR_ERR(timer_class);
        goto del_cdev;
    }

    /* create device */
    device_create(timer_class, NULL, dev_num, NULL, DEVICE_NAME);

    /* setup timer */
    timer_setup(&my_timer, timer_callback, 0);
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(5000));

    pr_info("Timer driver loaded\n");

    return 0;

del_cdev:
    cdev_del(&timer_cdev);

unregister:
    unregister_chrdev_region(dev_num, 1);
    return ret;
}

/* -------------------------------------------------- */
/* Module Exit                                        */
/* -------------------------------------------------- */

static void __exit timer_exit(void)
{
    del_timer_sync(&my_timer);

    device_destroy(timer_class, dev_num);
    class_destroy(timer_class);

    cdev_del(&timer_cdev);
    unregister_chrdev_region(dev_num, 1);

    pr_info("Timer driver removed\n");
}

module_init(timer_init);
module_exit(timer_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Isshika Sinha");
MODULE_DESCRIPTION("Timer interrupt driver");
