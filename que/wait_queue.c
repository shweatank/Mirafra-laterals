#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/hrtimer.h>
#include <linux/cdev.h>
#include <linux/ktime.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/wait.h>
#include <linux/timekeeping.h>

#define DEVICE_NAME "wait_queue"
#define TIME_INTERVAL_NS 50000000  //50 ms

static dev_t dev_num;
static struct cdev waitq_cdev;

static wait_queue_head_t wait_queue;
static int condition = 0;

static struct hrtimer my_timer;
static ktime_t interval;

/*Timer callback*/

static enum hrtimer_restart timer_callback(struct hrtimer* timer){
    condition = 1;
    wake_up_interruptible(&wait_queue);

    return HRTIMER_NORESTART;
}

/*Read operation*/
static ssize_t waitq_read(struct file *file, char __user *buf, size_t len, loff_t *offset){
    pr_info("Process going to sleep \n");

    /*Sleep untill condition becomes true*/

    wait_event_interruptible(wait_queue, condition != 0);

    condition = 0;

    pr_info("Process wake up \n");

    return 0;
}

static int waitq_open(struct inode* inode, struct file* file){
    return 0;
}

static int waitq_release(struct inode* inode, struct file* file){
    return 0;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = waitq_open,
    .read = waitq_read,
    .release = waitq_release,
};

/*Module init*/

static int __init waitq_driver_init(void){
    alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);

    cdev_init(&waitq_cdev, &fops);
    cdev_add(&waitq_cdev, dev_num, 1);

    init_waitqueue_head(&wait_queue);

    interval = ktime_set(0, TIME_INTERVAL_NS);

    hrtimer_init(&my_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
    my_timer.function = timer_callback;
    hrtimer_start(&my_timer, interval, HRTIMER_MODE_REL);

    pr_info("Wait queue driver loaded\n");
    pr_info("Major number : %d\n", MAJOR(dev_num));

    return 0;

}

static void __exit waitq_driver_exit(void){
    hrtimer_cancel(&my_timer);
    cdev_del(&waitq_cdev);
    unregister_chrdev_region(dev_num, 1);

    pr_info("Wait queue driver unloaded\n");

}

module_init(waitq_driver_init);
module_exit(waitq_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Narsimha");
MODULE_DESCRIPTION("Wait queue driver");