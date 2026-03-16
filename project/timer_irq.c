#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/timer.h>

#define DEVICE "timerdev"

static int major;
static int counter = 0;

static struct timer_list my_timer;

static void timer_func(struct timer_list *t)
{
    counter++;
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(2000));
}

static ssize_t dev_read(struct file *f,char __user *buf,size_t len,loff_t *off)
{
    char data[20];

    sprintf(data,"%d\n",counter);

    copy_to_user(buf,data,strlen(data));

    return strlen(data);
}

static struct file_operations fops={
.read = dev_read
};

static int __init timer_init(void)
{
    major = register_chrdev(0,DEVICE,&fops);

    timer_setup(&my_timer,timer_func,0);
    mod_timer(&my_timer,jiffies + msecs_to_jiffies(2000));

    printk("Timer Driver Loaded\n");

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
