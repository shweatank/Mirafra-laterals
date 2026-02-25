#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/io.h>

#define IRQ_NO   1

static unsigned char scancode;

static int dev_id;

static irqreturn_t keyboard_irq(int irq, void *dev)
{
    scancode = inb(0x60);

    return IRQ_WAKE_THREAD;
}

static irqreturn_t irq_threaded(int irq, void *dev)
{
    int a=10,b=20;

    if (scancode & 0x80) //concentrate on key press not on key release
        return IRQ_HANDLED;

    switch(scancode)
    {
        case 0x1E:pr_info("ADD = %d\n", a + b);
		  break;

        case 0x1F:pr_info("SUB = %d\n", a - b);
		  break;

	case 0x32:pr_info("MUL = %d\n", a * b);
		  break;
        case 0x20:pr_info("DIV = %d\n", a / b);
		  break;
    }

    return IRQ_HANDLED;
}

static int __init irq_init_thread(void)
{
    int ret;

    ret = request_threaded_irq(IRQ_NO,keyboard_irq,irq_threaded,IRQF_SHARED,"calci",&dev_id);

    if (ret) {
        pr_err("Failed to register IRQ %d\n", IRQ_NO);
        return ret;
    }

    pr_info("Threaded Keyboard IRQ Calculator Loaded\n");
    return 0;
}

static void __exit irq_exit_thread(void)
{
    free_irq(IRQ_NO, &dev_id);
    pr_info("Driver Removed\n");
}

module_init(irq_init_thread);
module_exit(irq_exit_thread);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Rani");
MODULE_DESCRIPTION("Threaded IRQ Keyboard Calculator");
