#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/device.h>

#define DEVICE_NAME "calc_ioctl_class"
#define CLASS_NAME  "calc_class"
#define DRV_NAME    "calc_device"

#define CALC_IOC_MAGIC 'B'
#define CALC_IOC_SEND_NAME _IOWR(CALC_IOC_MAGIC, 1, struct calc_req)

struct calc_req {
    char name[50];
};

static dev_t g_dev;
static struct cdev g_cdev;
static struct class  *g_class;
static struct device *g_device;

static long calc_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    struct calc_req req;

    if (cmd != CALC_IOC_SEND_NAME)
        return -EINVAL;

    if (copy_from_user(&req, (struct calc_req __user *)arg, sizeof(req)))
        return -EFAULT;

    pr_info("Kernel received : %s\n", req.name);

    // Kernel modifies data (loopback example)

    strcat(req.name, "\n");
    

    if (copy_to_user((struct calc_req __user *)arg, &req, sizeof(req)))
        return -EFAULT;

    return 0;
}

static struct file_operations calc_fops = {
    .owner          = THIS_MODULE,
    .unlocked_ioctl = calc_ioctl,
};

static int __init calc_init(void)
{
    int ret;

    ret = alloc_chrdev_region(&g_dev, 0, 1, DRV_NAME);
    if (ret)
        return ret;

    cdev_init(&g_cdev, &calc_fops);
    cdev_add(&g_cdev, g_dev, 1);

    g_class = class_create(CLASS_NAME);
    g_device = device_create(g_class, NULL, g_dev, NULL, DEVICE_NAME);

    pr_info("Driver loaded\n");
    return 0;
}

static void __exit calc_exit(void)
{
    device_destroy(g_class, g_dev);
    class_destroy(g_class);
    cdev_del(&g_cdev);
    unregister_chrdev_region(g_dev, 1);
    pr_info("Driver unloaded\n");
}

module_init(calc_init);
module_exit(calc_exit);

MODULE_LICENSE("GPL");
