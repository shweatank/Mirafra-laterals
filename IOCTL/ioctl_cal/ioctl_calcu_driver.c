#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/uaccess.h>
#include <linux/ioctl.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>

#define DEV_NAME        "ioctl_calu"
#define CALC_IOC_MAGIC  'B'

/* ===================== IOCTL STRUCT ===================== */

struct calc_req {
    int a;
    int b;
    long result;
    int err;
};

/* ===================== IOCTL COMMANDS ===================== */

#define CALC_IOC_ADD _IOWR(CALC_IOC_MAGIC, 1, struct calc_req)
#define CALC_IOC_SUB _IOWR(CALC_IOC_MAGIC, 2, struct calc_req)
#define CALC_IOC_MUL _IOWR(CALC_IOC_MAGIC, 3, struct calc_req)
#define CALC_IOC_DIV _IOWR(CALC_IOC_MAGIC, 4, struct calc_req)
#define CALC_IOC_MOD _IOWR(CALC_IOC_MAGIC, 5, struct calc_req)

/* ===================== GLOBALS ===================== */

static dev_t g_dev;
static struct class  *g_class;
static struct device *g_device;
static struct cdev    g_cdev;

/* ===================== IOCTL HANDLER ===================== */

static long basic_ioctl(struct file *file,
                        unsigned int cmd,
                        unsigned long arg)
{
    struct calc_req cal;

    if (_IOC_TYPE(cmd) != CALC_IOC_MAGIC)
        return -EINVAL;

    if (copy_from_user(&cal,
        (struct calc_req __user *)arg,
        sizeof(cal)))
        return -EFAULT;

    cal.err = 0;

    switch (cmd) {

    case CALC_IOC_ADD:
        cal.result = cal.a + cal.b;
        break;

    case CALC_IOC_SUB:
        cal.result = cal.a - cal.b;
        break;

    case CALC_IOC_MUL:
        cal.result = cal.a * cal.b;
        break;

    case CALC_IOC_DIV:
        if (cal.b == 0)
            cal.err = -EINVAL;
        else
            cal.result = cal.a / cal.b;
        break;

    case CALC_IOC_MOD:
        if (cal.b == 0)
            cal.err = -EINVAL;
        else
            cal.result = cal.a % cal.b;
        break;

    default:
        return -EINVAL;
    }

    if (copy_to_user((struct calc_req __user *)arg,
                     &cal,
                     sizeof(cal)))
        return -EFAULT;

    return 0;
}

/* ===================== FILE OPERATIONS ===================== */

static struct file_operations fops = {
    .owner          = THIS_MODULE,
    .unlocked_ioctl = basic_ioctl,
};

/* ===================== INIT ===================== */

static int __init basic_init(void)
{
    int ret;

    /* Allocate device number */
    ret = alloc_chrdev_region(&g_dev, 0, 1, DEV_NAME);
    if (ret)
        return ret;

    /* Initialize cdev */
    cdev_init(&g_cdev, &fops);

    ret = cdev_add(&g_cdev, g_dev, 1);
    if (ret)
        goto unregister_region;

    /* Create class (Kernel 6.x API) */
    g_class = class_create(DEV_NAME);
    if (IS_ERR(g_class)) {
        ret = PTR_ERR(g_class);
        goto del_cdev;
    }

    /* Create device node /dev/basic_ioctl */
    g_device = device_create(g_class,
                             NULL,
                             g_dev,
                             NULL,
                             DEV_NAME);

    if (IS_ERR(g_device)) {
        ret = PTR_ERR(g_device);
        goto destroy_class;
    }

    pr_info(DEV_NAME ": Loaded Major=%d Minor=%d\n",
            MAJOR(g_dev), MINOR(g_dev));

    return 0;

destroy_class:
    class_destroy(g_class);
del_cdev:
    cdev_del(&g_cdev);
unregister_region:
    unregister_chrdev_region(g_dev, 1);
    return ret;
}

/* ===================== EXIT ===================== */

static void __exit basic_exit(void)
{
    device_destroy(g_class, g_dev);
    class_destroy(g_class);
    cdev_del(&g_cdev);
    unregister_chrdev_region(g_dev, 1);

    pr_info(DEV_NAME ": Unloaded\n");
}

module_init(basic_init);
module_exit(basic_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Narsimha");
MODULE_DESCRIPTION("IOCTL Calculator Driver");
MODULE_VERSION("1.0");