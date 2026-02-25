#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>

#define TIMER_INTERVAL_SEC 1

static struct hrtimer hr_timer;
static ktime_t interval;
static int count = 0;

//Timer Interrupt Handler
static enum hrtimer_restart timer_callback(struct hrtimer *timer)
{
    count++;

    pr_info("Timer Interrupt Fired! Count = %d\n", count);

    /* Re-arm timer for periodic behavior */
    hrtimer_forward_now(timer, interval);

    return HRTIMER_RESTART;
}

static int __init timer_interrupt_init(void)
{
    pr_info("Timer Interrupt Module Loaded\n");

    /* Set interval = 1 second */
    interval = ktime_set(TIMER_INTERVAL_SEC, 0);

    /* Initialize timer */
    hrtimer_init(&hr_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
    hr_timer.function = timer_callback;

    /* Start timer */
    hrtimer_start(&hr_timer, interval, HRTIMER_MODE_REL);

    return 0;
}

static void __exit timer_interrupt_exit(void)
{
    hrtimer_cancel(&hr_timer);
    pr_info("Timer Interrupt Module Removed\n");
}

module_init(timer_interrupt_init);
module_exit(timer_interrupt_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Rani");
MODULE_DESCRIPTION("Real Timer Interrupt using hrtimer");
