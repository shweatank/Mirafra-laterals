#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/device.h>

/* ioctl magic */
#define CALC_MAGIC 'k'

/* shared structure */
struct calc_data {
    int a;
    int b;
    int result;
};

/* ioctl commands */
#define CALC_IOC_ADD _IOWR(CALC_MAGIC, 1, struct calc_data)
#define CALC_IOC_SUB _IOWR(CALC_MAGIC, 2, struct calc_data)
#define CALC_IOC_MUL _IOWR(CALC_MAGIC, 3, struct calc_data)
#define CALC_IOC_DIV _IOWR(CALC_MAGIC, 4, struct calc_data)

#define DEVICE_NAME "calcioct"
#define CLASS_NAME  "calcioct"

static dev_t dev_num;
static struct cdev calc_cdev;
static struct class *calc_class;

/* ioctl handler */
static long calc_ioctl(struct file *file,
                       unsigned int cmd,
                       unsigned long arg)
{
    struct calc_data data;

    if (copy_from_user(&data, (void __user *)arg, sizeof(data)))
        return -EFAULT;

    switch (cmd) {

        case CALC_IOC_ADD:
            data.result = data.a + data.b;
            break;

        case CALC_IOC_SUB:
            data.result = data.a - data.b;
            break;

        case CALC_IOC_MUL:
            data.result = data.a * data.b;
            break;

        case CALC_IOC_DIV:
            if (data.b == 0)
                return -EINVAL;
            data.result = data.a / data.b;
            break;

        default:
            return -EINVAL;
    }

    if (copy_to_user((void __user *)arg, &data, sizeof(data)))
        return -EFAULT;

    return 0;
}

static struct file_operations fops = {
    .owner          = THIS_MODULE,
    .unlocked_ioctl = calc_ioctl,
};

static int __init calc_init(void)
{
    alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);

    cdev_init(&calc_cdev, &fops);
    cdev_add(&calc_cdev, dev_num, 1);

    calc_class = class_create(CLASS_NAME);
    device_create(calc_class, NULL, dev_num, NULL, DEVICE_NAME);

    printk(KERN_INFO "Calc ioctl driver loaded\n");
    return 0;
}

static void __exit calc_exit(void)
{
    device_destroy(calc_class, dev_num);
    class_destroy(calc_class);
    cdev_del(&calc_cdev);
    unregister_chrdev_region(dev_num, 1);
}

module_init(calc_init);
module_exit(calc_exit);

MODULE_LICENSE("GPL");
