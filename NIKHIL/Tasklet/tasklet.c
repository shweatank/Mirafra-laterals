#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/interrupt.h>

#define KEYBOARD_IQR 1

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Nikhil");
MODULE_DESCRIPTION("keyboard IRQ + Tasklet (Linux 6.8 correct API)");

static void  keyboard_tasklet_fn(struct tasklet_struct *t);

DECLARE_TASKLET(keyboard_tasklet, keyboard_tasklet_fn);

//Tasklet(Bottom half)
static void keyboard_tasklet_fn(struct tasklet_struct *t)
{
    pr_info("tasklet: bottom half executed\n");
}

//IRQ Handler (Top half)

static irqreturn_t keyboard_irq_handler(int irq, void *dev_id)
{
    pr_info("irq: keyboard interrupt occured\n");
    tasklet_schedule(&keyboard_tasklet);
    return IRQ_HANDLED;
}


//init module
static int __init tasklet_irq_init(void)
{
    int ret;

    ret = request_irq(
            KEYBOARD_IQR,
            keyboard_irq_handler, 
            IRQF_SHARED,      /* share IRQ 1 with existing keyboard driver */
            "kbd_tasklet",
            (void *)keyboard_irq_handler);
        
     if(ret){
        pr_err("Failed to request IRQ &d\n",KEYBOARD_IQR);
        return ret;
     }       
     pr_info("tasklet module loaded\n");
     return 0;
} 


//EXIT Module

static void __exit tasklet_irq_exit(void)
{
    tasklet_kill(&keyboard_tasklet);
    free_irq(KEYBOARD_IQR,(void *)keyboard_irq_handler);

     pr_info("tasklet module unloaded\n");
}