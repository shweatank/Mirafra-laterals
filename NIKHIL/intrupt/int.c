#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/interrupt.h>



#define DRIVER_NAME "irq_Demo_driver"
#define IRQ_NUM 1
static int irq_counter = 0;




static irqreturn_t irq_demo_isr(int irq, void *dev_id)
{
irq_counter++;

pr_info("%s: Interrrupt recevied! RQ=%d count=%d",
        DRIVER_NAME, irq, irq_counter);

        return IRQ_HANDLED;
}

static int __init irq_demo_init(void)
{
    int ret;
    pr_info("%s: Initializing\n", DRIVER_NAME);
    /*
        request_irq arguments:
        irq ->IRQ number
        handler -> ISR function
        flags -> IRQF_SHARED allow sharing
        name -> isible in /proc/interrupt
    */
   ret = request_irq(IRQ_NUM,
                    irq_demo_isr,
                IRQF_SHARED,
            DRIVER_NAME,
            (void *)irq_demo_isr);
    if(ret)
    {
        pr_err("%s: Failed to request IRQ %d\n",DRIVER_NAME, IRQ_NUM);
        return ret;
    }
        pr_info("%s: IRQ %d register successfully\n", DRIVER_NAME,IRQ_NUM);
    return 0;
}

static void __exit irq_demo_exit(void)
{
        pr_info("%s: cleaning up\n", DRIVER_NAME);
    /*
    free_irq must match:
    --same IRQ number
    --same dev_id pointer
    */

        free_irq(IRQ_NUM,(void *)irq_demo_isr);
        pr_info("%s: IRQ freed\n",DRIVER_NAME);
}

module_init(irq_demo_init);
module_exit(irq_demo_exit);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("NIKHIL");
MODULE_DESCRIPTION("Simple linux kernel IRQ handling example");
