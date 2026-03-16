#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/timer.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/wait.h>

#define DEVICE_NAME "timer_isshika"
#define CLASS_NAME  "timer_class"

static struct timer_list my_timer;
static int flag = 0;

/* wait queue */
static wait_queue_head_t wq;

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

    /* wake up blocked readers */
    wake_up_interruptible(&wq);

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
    char msg[] = "Timer Event\n";
    int msg_len = strlen(msg);

    /* sleep until timer event */
    wait_event_interruptible(wq, flag != 0);

    flag = 0;

    if(copy_to_user(buf, msg, msg_len))
        return -EFAULT;

    return msg_len;
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

    init_waitqueue_head(&wq);

    ret = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
    if(ret)
        return ret;

    cdev_init(&timer_cdev, &fops);

    ret = cdev_add(&timer_cdev, dev_num, 1);
    if(ret)
        goto unregister;

    timer_class = class_create(CLASS_NAME);
    if(IS_ERR(timer_class))
    {
        ret = PTR_ERR(timer_class);
        goto del_cdev;
    }

    device_create(timer_class, NULL, dev_num, NULL, DEVICE_NAME);

    timer_setup(&my_timer, timer_callback, 0);
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(5000));

    pr_info("Timer driver loaded\n");

    return 0;

del_cdev:
    cdev_del(&timer_cdev);

unregister:
    unregister_chrdev_region(dev_num,1);
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
    unregister_chrdev_region(dev_num,1);

    pr_info("Timer driver removed\n");
}

module_init(timer_init);
module_exit(timer_exit);

MODULE_LICENSE("GPL");
