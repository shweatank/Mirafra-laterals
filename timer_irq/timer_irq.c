#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/interrupt.h>

#define DRIVER_NAME "timer_irq_driver"
#define TIMER_IRQ   0   /* System timer IRQ (x86 typical) */

static int irq_counter;
static char* devname = DRIVER_NAME;

/* ISR - Top Half */
static irqreturn_t timer_irq_isr(int irq, void *dev_id)
{
    irq_counter++;

    if (irq_counter % 1000 == 0) {
        pr_info("%s : Timer IRQ %d triggered %d times\n",
                DRIVER_NAME, irq, irq_counter);
    }

    return IRQ_HANDLED;
}

static int __init timer_irq_init(void)
{
    int ret;

    pr_info("%s : Loading module\n", DRIVER_NAME);

    ret = request_irq(TIMER_IRQ,
                      timer_irq_isr,
                      IRQF_SHARED,
                      devname,
                      &devname);

    if (ret) {
        pr_err("%s : Unable to request IRQ %d\n",
               DRIVER_NAME, TIMER_IRQ);
        return ret;
    }

    pr_info("%s : Successfully registered IRQ %d\n",
            DRIVER_NAME, TIMER_IRQ);

    return 0;
}

static void __exit timer_irq_exit(void)
{
    free_irq(TIMER_IRQ, &devname);
    pr_info("%s : IRQ freed and module unloaded\n", DRIVER_NAME);
}

module_init(timer_irq_init);
module_exit(timer_irq_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Adepu Shashank");
MODULE_DESCRIPTION("Shared Timer IRQ Example");
