#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/delay.h>

#define DRIVER_NAME "irq_threaded_demo"
#define IRQ_NUM 1   


static irqreturn_t irq_top(int irq, void *dev_id)
{
    pr_info("%s: Top half executed\n", DRIVER_NAME);


    return IRQ_WAKE_THREAD;
}


static irqreturn_t irq_thread(int irq, void *dev_id)
{
    pr_info("%s: Threaded IRQ handler running\n", DRIVER_NAME);


    msleep(50);

    return IRQ_HANDLED;
}


static int __init irq_threaded_init(void)
{
    int ret;

    pr_info("%s: Loading driver\n", DRIVER_NAME);

    ret = request_threaded_irq(IRQ_NUM,
                               irq_top,
                               irq_thread,
                               IRQF_SHARED,
                               DRIVER_NAME,
                               (void *)irq_thread);

    if (ret) {
        pr_err("%s: IRQ request failed\n", DRIVER_NAME);
        return ret;
    }

    pr_info("%s: IRQ registered\n", DRIVER_NAME);
    return 0;
}


static void __exit irq_threaded_exit(void)
{
    pr_info("%s: Unloading driver\n", DRIVER_NAME);

    free_irq(IRQ_NUM, (void *)irq_thread);

    pr_info("%s: IRQ freed\n", DRIVER_NAME);
}

module_init(irq_threaded_init);
module_exit(irq_threaded_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Demo");
MODULE_DESCRIPTION("Threaded IRQ example");
