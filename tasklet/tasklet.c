#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/interrupt.h>

#define KEYBOARD_IRQ 1

static void keyboard_tasklet_fn(struct tasklet_struct* t);

DECLARE_TASKLET(keyboard_tasklet, keyboard_tasklet_fn);

/*Bottom half*/
static void keyboard_tasklet_fn(struct tasklet_struct* t){
    pr_info("Tasklet bottom half executed\n");
}

/*Top Half*/
static irqreturn_t keyboard_irq_hanlder(int irq, void* dev_id){
    pr_info("IRQ : keyboard interrupt occured \n");

    tasklet_schedule(&keyboard_tasklet);

    return IRQ_HANDLED;
}

static int __init tasklet_irq_init(void){
    int ret;

    ret = request_irq(KEYBOARD_IRQ,
                    keyboard_irq_hanlder,
                    IRQF_SHARED,
                    "keyboard_tasklet",
                    (void *)keyboard_irq_hanlder);
    if(ret){
        pr_info("Failed to rrquest IRQ %d\n", KEYBOARD_IRQ);
        return ret;
    }

    pr_info("Tasklet module loaded\n");
    return 0;
}

static void __exit tasklet_irq_exit(void){
    tasklet_kill(&keyboard_tasklet);
    free_irq(KEYBOARD_IRQ, (void *)keyboard_irq_hanlder);

    pr_info("Tasklet module unloaded\n");
}


module_init(tasklet_irq_init);
module_exit(tasklet_irq_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Adepu Shashank");
MODULE_DESCRIPTION("Tasklet demo code ");