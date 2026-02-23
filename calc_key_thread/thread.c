#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/delay.h>

#define KBD_IRQ   1
#define KBD_DATA  0x60

static unsigned char scancode;

static irqreturn_t irq_top(int irq, void *dev_id)
{
	scancode = inb(KBD_DATA);
	
	return IRQ_WAKE_THREAD;
}

static irqreturn_t irq_thread(int irq, void *dev_id)
{
	int a = 20, b = 5;

	if (scancode & 0x80)
	{
		return IRQ_HANDLED;
	}
	switch (scancode)
	{
		case 0x1E:
			pr_info("ADD = %d\n", a + b);
			break;
		
		case 0x1F:
			pr_info("SUB = %d\n", a - b);
			break;
		
		case 0x32:
			pr_info("MUL = %d\n", a * b);
			break;

		case 0x20:
			pr_info("DIV = %d\n", a / b);
			break;
	}
	
	msleep(50);
	
	return IRQ_HANDLED;
}


static int __init irq_threaded_init(void)
{
	pr_info("Threaded Keyboard Calculator Loaded\n");
	return request_threaded_irq(KBD_IRQ, irq_top, irq_thread, IRQF_SHARED, "kbd_threaded_calc", (void *)irq_thread);
}

static void __exit irq_threaded_exit(void)
{
	free_irq(KBD_IRQ, (void *)irq_thread);
	
	pr_info("Driver Removed\n");
}

module_init(irq_threaded_init);
module_exit(irq_threaded_exit);

MODULE_LICENSE("GPL");
