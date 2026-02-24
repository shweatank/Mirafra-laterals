#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/ioctl.h>
#include <linux/uaccess.h>
#include <linux/interrupt.h>
#include <asm/io.h>

#define SC_A 0x1E
#define SC_S 0x1F
#define SC_M 0x32
#define SC_D 0x20



static long int file_ioctl(struct file *file,unsigned int cmd,unsigned long arg);

#define DEV_NAME "intr_driver"
#define DEVICE_NAME "INTR_DEVICE"
#define Int_name "INTR_NAME"

struct input_output{
    int a;
    int b;
    int result;
};

static struct input_output io;

#define MAGIC_NUM 'B'
#define SET_DATA _IOW(MAGIC_NUM, 1, struct input_output)
#define GET_DATA _IOR(MAGIC_NUM, 2, struct input_output)

#define IRQ_NUM 1


unsigned char scancode;


dev_t dev;
static struct class *dev_class;
static struct cdev cdev_t;


static struct file_operations fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = file_ioctl
};

//IRQ HANDLER

static irqreturn_t IRQ_top(int irq,void *dev_id){
    
    
    scancode = inb(0x60);

    if (scancode & 0x80)
        return IRQ_HANDLED;

    if(scancode == SC_A || scancode == SC_S || scancode == SC_M || scancode == SC_D){
        return IRQ_WAKE_THREAD;
    }


    return IRQ_HANDLED;
}

static irqreturn_t IRQ_bot(int irq,void *dev_id){

    io.a = 10;
    io.b = 2;
    switch (scancode)
    {
        case 0x1E:   // A key
            io.result = io.a + io.b;
            pr_info("Addition Done: %d\n", io.result);
            break;

        case 0x1F:   // S key
            io.result = io.a - io.b;
            pr_info("Subtraction Done: %d\n", io.result);
            break;

        case 0x32:   // M key
            io.result = io.a * io.b;
            pr_info("Addition Done: %d\n", io.result);
            break;

        case 0x20:   // S key
            io.result = io.a / io.b;
            pr_info("Subtraction Done: %d\n", io.result);
            break;

        default:
            return IRQ_HANDLED;

    }
    return IRQ_HANDLED;
}

//IOCTL Function

static long file_ioctl(struct file *file,unsigned int cmd,unsigned long arg){
    
     switch (cmd)
    {
        case SET_DATA:
            if (copy_from_user(&io,
                               (void __user *)arg,
                               sizeof(io)))
                return -EFAULT;

            pr_info("Received: %d %d\n", io.a, io.b);
            break;

        case GET_DATA:
            if (copy_to_user((void __user *)arg,
                             &io,
                             sizeof(io)))
                return -EFAULT;
            break;
        
        default:
            return -EINVAL;
    }
    return 0;
}

static int __init intr_driver_init(void){

    int ret = alloc_chrdev_region(&dev,0,1,DEV_NAME);
    if(ret<0){
        pr_info("Error at ret\n");
        return -1;
    }

    cdev_init(&cdev_t,&fops);

    dev_class = class_create("Dev_Class");

    device_create(dev_class,NULL,dev,NULL,DEVICE_NAME);
    
    cdev_add(&cdev_t,dev,1);

    //IRQ THREADS

    int irq_ret = request_threaded_irq(IRQ_NUM,
                                        IRQ_top,
                                        IRQ_bot,
                                        IRQF_SHARED,
                                        Int_name,(void *)IRQ_bot);

    if(irq_ret){
        pr_err("%s: Filed to request IRQ %d\n",Int_name,IRQ_NUM);
        return irq_ret;
    }

    return 0;
}

static void __exit intr_driver_exit(void){
    free_irq(IRQ_NUM, (void *)IRQ_top);
    cdev_del(&cdev_t);
    device_destroy(dev_class,dev);
    class_destroy(dev_class);
    unregister_chrdev_region(dev,1);
    pr_info("Driver exited\n");
}

module_init(intr_driver_init);
module_exit(intr_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Narsimha");
MODULE_DESCRIPTION("Keys calculation");
MODULE_VERSION("1.0");