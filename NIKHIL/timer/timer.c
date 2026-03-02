#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/timer.h>
#include <linux/jiffies.h>

static struct timer_list my_timer;

static void timer_callback(struct timer_list *t)
{
    printk(KERN_INFO "Timer expired!\n");
}

static int __init timer_module_init(void)
{
    printk(KERN_INFO "Timer module loaded\n");
    timer_setup(&my_timer, timer_callback, 0);
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(5000));
    return 0;
}

static void __exit timer_module_exit(void)
{
    del_timer_sync(&my_timer);
    printk(KERN_INFO "Timer module unloaded\n");
}

module_init(timer_module_init);
module_exit(timer_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Nikhil");
MODULE_DESCRIPTION("Basic Linux Kernel Timer Example");