#include <linux/module.h>
#include <linux/init.h>
#include <linux/timer.h>
#include <linux/jiffies.h>

static struct timer_list my_timer;

static void timer_callback(struct timer_list *t)
{
    pr_info("Timer fired\n");
    mod_timer(t, jiffies + msecs_to_jiffies(1000));
}

static int __init my_init(void)
{
    timer_setup(&my_timer, timer_callback, 0);
    mod_timer(&my_timer, jiffies + msecs_to_jiffies(1000));
    return 0;
}

static void __exit my_exit(void)
{
    timer_shutdown_sync(&my_timer);   // <-- USE THIS IN 6.x
}

module_init(my_init);
module_exit(my_exit);

MODULE_LICENSE("GPL");