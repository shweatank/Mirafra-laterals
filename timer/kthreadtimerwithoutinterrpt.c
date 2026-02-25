#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/kthread.h>
#include <linux/delay.h>

static struct task_struct *timer_thread;

static int thread_fn(void *data)
{
    while (!kthread_should_stop())
    {
        pr_info("Timer interrupt triggered\n");
        msleep(5000);
    }

    return 0;
}

static int __init timer_init(void)
{
    pr_info("Timer Thread Module Loaded\n");

    timer_thread = kthread_run(thread_fn,NULL,"timer_thread");

    if (IS_ERR(timer_thread))
        return PTR_ERR(timer_thread);

    return 0;
}

static void __exit timer_exit(void)
{
    if (timer_thread)
        kthread_stop(timer_thread);

    pr_info("Timer Thread Module Removed\n");
}

module_init(timer_init);
module_exit(timer_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Rani");
MODULE_DESCRIPTION("Simple Timer using Kernel Thread");
