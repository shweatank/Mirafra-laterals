#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "Calci"

static int major_number;
static int result;
static int data_available = 0;

struct calc_data {
    int num1;
    int num2;
    char op;
};

static int calc_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "Calculator: Device opened\n");
    return 0;
}

static int calc_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "Calculator: Device closed\n");
    return 0;
}

static ssize_t calc_write(struct file *file,
                          const char __user *user_buf,
                          size_t count,
                          loff_t *offset)
{
    struct calc_data data;

    if (copy_from_user(&data, user_buf, sizeof(data)))
        return -EFAULT;

    switch (data.op)
    {
        case '+':
            result = data.num1 + data.num2;
            break;
        case '-':
            result = data.num1 - data.num2;
            break;
        case '*':
            result = data.num1 * data.num2;
            break;
        case '/':
            if (data.num2 == 0)
                return -EINVAL;
            result = data.num1 / data.num2;
            break;
        default:
            return -EINVAL;
    }

    data_available = 1;

    printk(KERN_INFO "Calc: %d %c %d = %d\n",
           data.num1, data.op, data.num2, result);

    return sizeof(data);
}

static ssize_t calc_read(struct file *file,
                         char __user *user_buf,
                         size_t count,
                         loff_t *offset)
{
    if (!data_available)
        return 0;

    if (copy_to_user(user_buf, &result, sizeof(result)))
        return -EFAULT;

    data_available = 0;
    return sizeof(result);
}

static struct file_operations calc_fops = {
    .owner = THIS_MODULE,
    .open = calc_open,
    .write = calc_write,
    .read = calc_read,
    .release = calc_release,
};

static int __init calc_init(void)
{
    major_number = register_chrdev(0, DEVICE_NAME, &calc_fops);

    if (major_number < 0)
        return major_number;

    printk(KERN_INFO "Calculator Driver Loaded\n");
    printk(KERN_INFO "mknod /dev/%s c %d 0\n",
           DEVICE_NAME, major_number);

    return 0;
}

static void __exit calc_exit(void)
{
    unregister_chrdev(major_number, DEVICE_NAME);
    printk(KERN_INFO "Calculator Driver Unloaded\n");
}

module_init(calc_init);
module_exit(calc_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Rani");
MODULE_DESCRIPTION("Kernel Calculator Driver");
