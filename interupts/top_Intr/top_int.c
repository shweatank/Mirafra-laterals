#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/delay.h>

#define DRIVER_NAME "irq_threaded_demo"
#define IRQ_NUM     1   /* Keyboard IRQ on x86 */


/* Top half */
static irqreturn_t irq_top(int irq, void *dev_id)
{
    /* Top half: fast, no sleeping */
    return IRQ_WAKE_THREAD;
}

/* Bottom half */
static irqreturn_t irq_thread(int irq, void *dev_id)
{
    pr_info(DRIVER_NAME ": Threaded IRQ handler (can sleep)\n");

    msleep(50);   /* Sleeping allowed in threaded handler */

    return IRQ_HANDLED;
}

static int __init irq_threaded_init(void)
{
    int ret;

    pr_info(DRIVER_NAME ": Initializing\n");

    ret = request_threaded_irq(IRQ_NUM,
                               irq_top,      /* top half */
                               irq_thread,   /* threaded bottom half */
                               IRQF_SHARED,
                               DRIVER_NAME,
                               &irq_thread); /* unique dev_id */

    if (ret) {
        pr_err(DRIVER_NAME ": Failed to request IRQ %d\n", IRQ_NUM);
        return ret;
    }

    return 0;
}

static void __exit irq_threaded_exit(void)
{
    pr_info(DRIVER_NAME ": Cleaning up\n");

    free_irq(IRQ_NUM, &irq_thread);

    pr_info(DRIVER_NAME ": IRQ freed\n");
}

module_init(irq_threaded_init);
module_exit(irq_threaded_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Narsimha");
MODULE_DESCRIPTION("Threaded IRQ Demo Driver");