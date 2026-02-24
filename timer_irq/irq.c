#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/timer.h>
#include <linux/jiffies.h>

#define TIMEOUT 1000   // milliseconds

static struct timer_list my_timer;
static int count;

/* -------- TIMER HANDLER -------- */
static void timer_callback(struct timer_list *t)
{
    count++;

    pr_info("Timer IRQ Fired : %d\n", count);

	/* restart timer */
	mod_timer(&my_timer, jiffies + msecs_to_jiffies(TIMEOUT));
}

/* -------- INIT -------- */
static int __init timer_init(void)
{
	pr_info("Timer IRQ Demo Loaded\n");
	
	/*initialize time */
	timer_setup(&my_timer, timer_callback, 0);
	
	/*start timer*/
	mod_timer(&my_timer, jiffies + msecs_to_jiffies(TIMEOUT));
	
	return 0;
}


static void __exit timer_exit(void)
{
    del_timer_sync(&my_timer);

    pr_info("Timer IRQ Demo Removed\n");
}

module_init(timer_init);
module_exit(timer_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Karthik");
MODULE_DESCRIPTION("Simple Timer IRQ Demonstration");
