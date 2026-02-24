#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/io.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/err.h>
#define KBD_IRQ   1
#define KBD_DATA  0x60

#define SIZE	256

#define DRV_NAME	"cal_key_user"
#define CLS_NAME	"cal_user_class"
#define DEV_NAME	"cal_user_class"

struct calc_req
{
	int num1;
	int num2;
	long result;
};

static int buf_size;

static struct calc_req cal;

static dev_t g_dev;
static struct cdev g_cdev;
static struct class *g_class;
static struct device *g_device;

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

static int cal_open(struct inode *inode, struct file *file)
{
        printk(KERN_INFO"cal_key_user : device opened\n");
        return 0;
}

static int cal_close(struct inode *inode, struct file *file)
{
        printk(KERN_INFO"cal_key_user : device closed\n");
        return 0;
}


static ssize_t cal_read(struct file *file, char __user *buf, size_t count, loff_t *offset)
{
        int bytes_to_copy;

        if(*offset >= buf_size)
        {
                return 0;
        }

        bytes_to_copy = min(count, (size_t) (buf_size - *offset));

        if(copy_to_user(buf, &cal, sizeof(struct calc_req)))
        {
                return -EFAULT;
        }

        *offset += bytes_to_copy;

        printk(KERN_INFO"cal_key_user : read %d bytes\n", bytes_to_copy);

        return bytes_to_copy;
}

static ssize_t cal_write(struct file *file, const char __user *buf, size_t count, loff_t *offset)
{
        int bytes_to_copy;

        bytes_to_copy = min(count, (size_t) SIZE);

        if(copy_from_user(&cal, buf, sizeof(struct calc_req)))
        {
                return -EFAULT;
        }

        cal.result = cal.num1 + cal.num2;

        buf_size = bytes_to_copy;

        printk(KERN_INFO"cal_key_user : written %d bytes\n", bytes_to_copy);

        return bytes_to_copy;
}

static struct file_operations fops = {
	.owner	= THIS_MODULE,
	.open	= cal_open,
	.release= cal_close,
	.read	= cal_read,
	.write	= cal_write
};

static int __init irq_threaded_init(void)
{
	int ret;

        ret = alloc_chrdev_region(&g_dev, 0, 1, DRV_NAME);

        if(ret)
        {
                pr_err(DRV_NAME ": alloc_chr_dev_region failed: %d\n", ret);
                return ret;
        }

        cdev_init(&g_cdev, &fops);
        //g_dev.owner = THIS_MODULE;

        ret = cdev_add(&g_cdev, g_dev, 1);

        if(ret)
        {
                pr_err(DRV_NAME ": cdev_add failed: %d\n", ret);
                goto unregister;
        }

        g_class = class_create(CLS_NAME);

        if(IS_ERR(g_class))
        {
                ret = PTR_ERR(g_class);
                pr_err(DRV_NAME ": class_create failed: %d\n", ret);
                goto device;
        }

        g_device = device_create(g_class, NULL, g_dev, NULL, DEV_NAME);

	if(IS_ERR(g_device))
        {
                ret = PTR_ERR(g_device);
                pr_err(DRV_NAME ": device_create failed: %d\n", ret);
                goto class;
        }

        pr_info(DRV_NAME ": loaded, /dev/%s major = %d minor = %d\n", DEV_NAME, MAJOR(g_dev), MINOR(g_dev));

	pr_info("Threaded Keyboard Calculator Loaded\n");
	
	return request_threaded_irq(KBD_IRQ, irq_top, irq_thread, IRQF_SHARED, "kbd_threaded_calc", (void *)irq_thread);

class:
        class_destroy(g_class);
device:
        cdev_del(&g_cdev);
unregister:
        unregister_chrdev_region(g_dev, 1);
        return ret;

}

static void __exit irq_threaded_exit(void)
{
	free_irq(KBD_IRQ, (void *)irq_thread);
	device_destroy(g_class, g_dev);
	class_destroy(g_class);
	cdev_del(&g_cdev);
	unregister_chrdev_region(g_dev, 1);	
	pr_info("Driver Removed\n");
}

module_init(irq_threaded_init);
module_exit(irq_threaded_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("KARTHIK");
