#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/timer.h>
#include <linux/wait.h>
#include <linux/poll.h>

#define DEVICE "timerdev"

static int major;
static struct timer_list my_timer;

static wait_queue_head_t wq;
static int event_flag = 0;

static void timer_func(struct timer_list *t)
{
    event_flag = 1;

    wake_up_interruptible(&wq);

    mod_timer(&my_timer, jiffies + msecs_to_jiffies(8000));
}

static ssize_t dev_read(struct file *f,char __user *buf,size_t len,loff_t *off)
{
    char msg[]="timer\n";

    if(event_flag == 0)
        return 0;

    event_flag = 0;

    if(copy_to_user(buf,msg,sizeof(msg)))
        return -EFAULT;

    return sizeof(msg);
}

static __poll_t dev_poll(struct file *file, poll_table *wait)
{
    poll_wait(file,&wq,wait);

    if(event_flag)
        return POLLIN;

    return 0;
}

static struct file_operations fops={
.read = dev_read,
.poll = dev_poll
};

static int __init timer_init(void)
{
    major = register_chrdev(0,DEVICE,&fops);

    init_waitqueue_head(&wq);

    timer_setup(&my_timer,timer_func,0);

    mod_timer(&my_timer,jiffies + msecs_to_jiffies(5000));

    printk("Timer driver loaded major=%d\n",major);

    return 0;
}

static void __exit timer_exit(void)
{
    del_timer(&my_timer);

    unregister_chrdev(major,DEVICE);
}

module_init(timer_init);
module_exit(timer_exit);

MODULE_LICENSE("GPL");
