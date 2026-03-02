#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <asm/io.h>

#define DEVICE "kbdcalc"
#define KBD_IRQ 1

static int major;
static int num1, num2;
static unsigned char last_scancode;

/* scan codes */
#define KEY_A 0x1E
#define KEY_S 0x1F
#define KEY_D 0x20
#define KEY_M 0x32


static irqreturn_t kbd_top(int irq, void *dev_id)
{
    last_scancode = inb(0x60);

    if (last_scancode & 0x80)   
        return IRQ_HANDLED;

    return IRQ_WAKE_THREAD;    
}


static irqreturn_t kbd_thread(int irq, void *dev_id)
{

    switch(last_scancode)
    {
        case KEY_A:
            printk(KERN_INFO "ADD = %d\n", num1 + num2);
            break;

        case KEY_S:
            printk(KERN_INFO "SUB = %d\n", num1 - num2);
            break;

        case KEY_M:
            printk(KERN_INFO "MUL = %d\n", num1 * num2);
            break;

        case KEY_D:
            if(num2 != 0)
                printk(KERN_INFO "DIV = %d\n", num1 / num2);
            else
                printk(KERN_INFO "DIV by zero\n");
            break;
    }

    return IRQ_HANDLED;
}


static ssize_t dev_write(struct file *f,
                         const char __user *buf,
                         size_t len, loff_t *off)
{
    char kbuf[50];

    if(copy_from_user(kbuf, buf, len))
        return -EFAULT;

    kbuf[len] = '\0';

    sscanf(kbuf, "%d %d", &num1, &num2);

    printk(KERN_INFO "Numbers received: %d %d\n", num1, num2);
    printk(KERN_INFO "Press a(add) s(sub) m(mul) d(div)\n");

    return len;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .write = dev_write,
};


static int __init start(void)
{
    major = register_chrdev(0, DEVICE, &fops);
    printk(KERN_INFO "Major=%d\n", major);
    printk(KERN_INFO "driver loaded\n");

    request_threaded_irq(KBD_IRQ,
                         kbd_top,
                         kbd_thread,
                         IRQF_SHARED,
                         DEVICE,
                         (void*)kbd_thread);

    return 0;
}


static void __exit end(void)
{
    free_irq(KBD_IRQ, (void*)kbd_thread);
    unregister_chrdev(major, DEVICE);
    printk(KERN_INFO "driver unloaded\n");
}

module_init(start);
module_exit(end);
MODULE_LICENSE("GPL");
