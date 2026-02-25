#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/wait.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "waitq_dev"
#define TIMER_INTERVAL 500000000 

static dev_t dev_num;
static struct cdev waitq_cdev;

static wait_queue_head_t wait_queue;
static int condition =0;

static struct hrtimer my_timer;
static ktime_t interval;

static enum hrtimer_restart timer_callback(struct hrtimer *timer){
    condition=1;
    wake_up_interruptible(&wait_queue);
    return HRTIMER_NORESTART;
}

static ssize_t waitq_read(struct file *file, char __user *buf,size_t len,loff_t *offset)
{
    printk(KERN_INFO "Process is going to sleep-->\n");
    wait_event_interruptible(wait_queue,condition!=0);condition=0;
    printk(KERN_INFO "Process woje up\n");
    return 0;
}

static int waitq_open(struct inode *inode, struct file *file){
    return 0;
}

static int waitq_release(struct inode *inode, struct file *file){
    return 0;
}

static struct file_operations fops ={
    .owner =THIS_MODULE,
    .open = waitq_open,
    .read = waitq_read,
    .release= waitq_release,
};

static int __init waitq_driver_init(void)


{

    alloc_chrdev_region(&dev_num,0,1,DEVICE_NAME);
    cdev_init(&waitq_cdev,&fops);
    cdev_add(&waitq_cdev,dev_num,1);
    init_waitqueue_head(&wait_queue);
    interval = ktime_set(0,TIMER_INTERVAL);
    hrtimer_init(&my_timer,CLOCK_MONOTONIC,HRTIMER_MODE_REL);
    my_timer.function = timer_callback;
    hrtimer_start(&my_timer,interval,HRTIMER_MODE_REL);

    printk(KERN_INFO "waitqueue driver loaded with major number: %d\n",MAJOR(dev_num));
    return 0;
}

static void __exit waitq_driver_exit(void){
    hrtimer_cancel(&my_timer);
    cdev_del(&waitq_cdev);
    unregister_chrdev_region(dev_num,1);
    printk(KERN_INFO "Waitqueue Driver unloaded\n");
    
}
module_init(waitq_driver_init);
module_exit(waitq_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ISSHIKA SINHA");
MODULE_DESCRIPTION("A waitqueue implemented driver");