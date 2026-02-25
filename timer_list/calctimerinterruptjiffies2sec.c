#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/timer.h>
#include <linux/jiffies.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("TechDhaba");
MODULE_DESCRIPTION("Basic Linux Kernel Timer using jiffies");
MODULE_VERSION("1.0");

static struct timer_list my_timer;

/* Timer callback function */
static void my_timer_callback(struct timer_list *t)
{
    printk(KERN_INFO "Timer fired! jiffies = %lu\n", jiffies);

    /* Rearm timer to fire again after 2 seconds */
    mod_timer(&my_timer, jiffies + 2 * HZ);
}

/* Module init */
static int __init timer_init(void)
{
    printk(KERN_INFO "Timer module loaded\n");
    printk(KERN_INFO "HZ = %d\n", HZ);

    /* Initialize timer */
    timer_setup(&my_timer, my_timer_callback, 0);

    /* Start timer after 2 seconds */
    mod_timer(&my_timer, jiffies + 2 * HZ);

    return 0;
}

/* Module exit */
static void __exit timer_exit(void)
{
    del_timer_sync(&my_timer);
    printk(KERN_INFO "Timer module unloaded\n");
}

module_init(timer_init);
module_exit(timer_exit);
