#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>
#include <linux/wait.h>
#include <linux/poll.h>

#define DEVICE "timerdev"

static int major;
static struct hrtimer hr_timer;
static ktime_t interval;

static wait_queue_head_t wq;
static int flag = 0;

/* timer callback */
static enum hrtimer_restart timer_callback(struct hrtimer *timer)
{
	    printk("hrtimer fired\n");
flag = 1;

wake_up_interruptible(&wq);

hrtimer_forward_now(timer, interval);

return HRTIMER_RESTART;

}

/* read function */
static ssize_t dev_read(struct file *file,
char __user *buf,
size_t len,
loff_t *off)
{
char msg[] = "tick\n";

if(flag == 0)
    return 0;

flag = 0;

if(copy_to_user(buf,msg,sizeof(msg)))
    return -EFAULT;

return sizeof(msg);

}

/* poll function */
static __poll_t dev_poll(struct file *file, poll_table *wait)
{
    __poll_t mask = 0;

    poll_wait(file,&wq,wait);

    if(flag)
        mask |= POLLIN;

    return mask;
}

static struct file_operations fops = {
.owner = THIS_MODULE,
.read  = dev_read,
.poll  = dev_poll
};

static int __init timer_init(void)
{
major = register_chrdev(0,DEVICE,&fops);

init_waitqueue_head(&wq);

interval = ktime_set(5,0);   // 5 seconds

hrtimer_init(&hr_timer,CLOCK_MONOTONIC,HRTIMER_MODE_REL);

hr_timer.function = timer_callback;

hrtimer_start(&hr_timer,interval,HRTIMER_MODE_REL);

printk("hrtimer driver loaded major=%d\n",major);

return 0;
}

static void __exit timer_exit(void)
{
hrtimer_cancel(&hr_timer);

unregister_chrdev(major,DEVICE);

printk("hrtimer driver removed\n");
}

module_init(timer_init);
module_exit(timer_exit);

MODULE_LICENSE("GPL");

