#include<linux/module.h>
#include<linux/init.h>
#include<linux/kernel.h>
#include<linux/interrupt.h>

#define KBD_IRQ		1

MODULE_LICENSE("GPL");
MODULE_AUTHOR("KARTHIK");
MODULE_DESCRIPTION("Keyboard IRQ + Tasklet");

static void kbd_tasklet_fn(struct tasklet_struct *t);

DECLARE_TASKLET(kbd_tasklet, kbd_tasklet_fn);

static void kbd_tasklet_fn(struct tasklet_struct *t)
{
	pr_info("tasklet: bottom half executed\n");
}

static irqreturn_t kbd_irq_handler(int irq, void *dev_id)
{
	pr_info("irq: keyboard interrupt occured\n");

	tasklet_schedule(&kbd_tasklet);
	
	return IRQ_HANDLED;
}

static int __init tasklet_irq_init(void)
{
	int ret;

	ret = request_irq(KBD_IRQ, kbd_irq_handler, IRQF_SHARED, "kbd_tasklet", (void *)kbd_irq_handler);

	if(ret)
	{
		pr_err("Failed to request irq %d\n", KBD_IRQ);
		return ret;
	}

	pr_info("tasklet module loaded\n");

	return 0;
}

static void __exit tasklet_irq_exit(void)
{
	tasklet_kill(&kbd_tasklet);
	free_irq(KBD_IRQ, (void *)kbd_irq_handler);

	pr_info("tasklet module removed\n");
}

module_init(tasklet_irq_init);
module_exit(tasklet_irq_exit);
