#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/spinlock.h>
#include <linux/io.h>
#include <linux/wait.h>

#define IRQ_NO 1
#define DEVICE_NAME "irq_calci"

static int major;
static struct cdev calci_cdev;

static int num1 = 0, num2 = 0;
static int result = 0;
static unsigned char scancode;

static int dev_id;
static spinlock_t lock;

static wait_queue_head_t wq;
static int result_ready = 0;

/* ---------------- File Operations ---------------- */

static ssize_t calci_write(struct file *file,
                           const char __user *buf,
                           size_t len, loff_t *off)
{
    int data[2];

    if (len < sizeof(data))
        return -EINVAL;

    if (copy_from_user(data, buf, sizeof(data)))
        return -EFAULT;

    spin_lock(&lock);
    num1 = data[0];
    num2 = data[1];
    result_ready = 0;   // Reset flag
    spin_unlock(&lock);

    pr_info("Numbers received: %d %d\n", num1, num2);

    return sizeof(data);
}

static ssize_t calci_read(struct file *file,
                          char __user *buf,
                          size_t len, loff_t *off)
{
    /* Block until result is ready */
    wait_event_interruptible(wq, result_ready == 1);

    if (copy_to_user(buf, &result, sizeof(result)))
        return -EFAULT;

    spin_lock(&lock);
    result_ready = 0;  // reset after read
    spin_unlock(&lock);

    return sizeof(result);
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = calci_read,
    .write = calci_write,
};

/* ---------------- IRQ Handlers ---------------- */

static irqreturn_t keyboard_irq(int irq, void *dev)
{
    scancode = inb(0x60);
    return IRQ_WAKE_THREAD;
}

static irqreturn_t irq_threaded(int irq, void *dev)
{
    int a, b;

    if (scancode & 0x80)   // ignore key release
        return IRQ_HANDLED;

    spin_lock(&lock);
    a = num1;
    b = num2;
    spin_unlock(&lock);

    switch (scancode)
    {
        case 0x1E:   // A
            result = a + b;
            break;

        case 0x1F:   // S
            result = a - b;
            break;

        case 0x32:   // M
            result = a * b;
            break;

        case 0x20:   // D
            if (b != 0)
                result = a / b;
            else {
                pr_info("Division by zero avoided\n");
                result = 0;
            }
            break;

        default:
            return IRQ_HANDLED;
    }

    spin_lock(&lock);
    result_ready = 1;
    spin_unlock(&lock);

    wake_up_interruptible(&wq);

    pr_info("Result calculated: %d\n", result);

    return IRQ_HANDLED;
}

/* ---------------- Init ---------------- */

static int __init irq_init_thread(void)
{
    int ret;

    spin_lock_init(&lock);
    init_waitqueue_head(&wq);

    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0)
        return major;

    cdev_init(&calci_cdev, &fops);
    calci_cdev.owner = THIS_MODULE;
    cdev_add(&calci_cdev, MKDEV(major, 0), 1);

    ret = request_threaded_irq(IRQ_NO,
                               keyboard_irq,
                               irq_threaded,
                               IRQF_SHARED,
                               "calci",
                               &dev_id);

    if (ret) {
        unregister_chrdev(major, DEVICE_NAME);
        return ret;
    }

    pr_info("Stable IRQ Calculator Loaded. Major=%d\n", major);
    return 0;
}

/* ---------------- Exit ---------------- */

static void __exit irq_exit_thread(void)
{
    free_irq(IRQ_NO, &dev_id);
    cdev_del(&calci_cdev);
    unregister_chrdev(major, DEVICE_NAME);
    pr_info("Driver Removed\n");
}

module_init(irq_init_thread);
module_exit(irq_exit_thread);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Rani");
MODULE_DESCRIPTION("Threaded IRQ Keyboard Calculator - Stable Version");
