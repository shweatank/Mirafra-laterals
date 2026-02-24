#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>

#define INTERVAL_MS 1000

static struct hrtimer hr_timer;
static ktime_t kt;
static int count;

/* ---------- TIMER CALLBACK ---------- */
static enum hrtimer_restart timer_callback(struct hrtimer *timer)
{
    count++;

    pr_info("HR Timer Fired : %d\n", count);

    /* forward timer */
    hrtimer_forward_now(timer, kt);

    return HRTIMER_RESTART;
}

/* ---------- INIT ---------- */
static int __init hrtimer_init(void)
{
    pr_info("HR Timer Driver Loaded\n");

    /* interval */
    kt = ktime_set(0, INTERVAL_MS * 1000000);

    /* initialize timer */
    hrtimer_init(&hr_timer, CLOCK_MONOTONIC,
                 HRTIMER_MODE_REL);

    hr_timer.function = timer_callback;

    /* start timer */
    hrtimer_start(&hr_timer, kt, HRTIMER_MODE_REL);

    return 0;
}

/* ---------- EXIT ---------- */
static void __exit hrtimer_exit(void)
{
    hrtimer_cancel(&hr_timer);

    pr_info("HR Timer Driver Removed\n");
}

module_init(hrtimer_init);
module_exit(hrtimer_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Karthik");
MODULE_DESCRIPTION("Simple High Resolution Timer Demo");
