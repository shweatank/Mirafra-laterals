#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/interrupt.h>
#include <linux/mutex.h>
#include <asm/io.h>

#define DEVICE "diskeycal"
#define KBD_IRQ 1

static int major;
static int num1, num2;
static int result;
static int result_ready = 0;

static DEFINE_MUTEX(lock);

static unsigned char scancode;


#define KEY_A 0x1E
#define KEY_S 0x1F
#define KEY_D 0x20
#define KEY_M 0x32


static irqreturn_t kbd_top(int irq, void *dev_id)
{
    scancode = inb(0x60);

    if (scancode & 0x80)  
        return IRQ_HANDLED;

    return IRQ_WAKE_THREAD;
}


static irqreturn_t kbd_thread(int irq, void *dev_id)
{
    mutex_lock(&lock);

    switch(scancode)
    {
        case KEY_A:
            result = num1 + num2;
            printk(KERN_INFO "ADD = %d\n", result);
            break;

        case KEY_S:
            result = num1 - num2;
            printk(KERN_INFO "SUB = %d\n", result);
            break;

        case KEY_M:
            result = num1 * num2;
            printk(KERN_INFO "MUL = %d\n", result);
            break;

        case KEY_D:
            if(num2 != 0)
                result = num1 / num2;
            else {
                printk(KERN_INFO "DIV by zero\n");
                mutex_unlock(&lock);
                return IRQ_HANDLED;
            }
            printk(KERN_INFO "DIV = %d\n", result);
            break;

        default:
            mutex_unlock(&lock);
            return IRQ_HANDLED;
    }

    result_ready = 1;
    mutex_unlock(&lock);

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


static ssize_t dev_read(struct file *f,
                        char __user *buf,
                        size_t len, loff_t *off)
{
    char out[50];
    int ret;

    if(!result_ready)
        return 0;   

    mutex_lock(&lock);

    ret = sprintf(out, "Result = %d\n", result);
    result_ready = 0;

    mutex_unlock(&lock);

    return simple_read_from_buffer(buf, len, off, out, ret);
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .write = dev_write,
    .read  = dev_read,
};


static int __init start(void)
{
    major = register_chrdev(0, DEVICE, &fops);
    printk(KERN_INFO "driver loaded\n");
    printk(KERN_INFO "kbdcalc Major=%d\n", major);

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
