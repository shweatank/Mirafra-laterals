#include<linux/module.h>
#include<linux/fs.h>
#include<linux/uaccess.h>

#define DEVICE_NAME "basic_ioctl"

#define MAGIC 'a' 
//enum or macro, taking _IO, _IOR, _IOW, _IOWR macros encode the magic number into ioctl command,Using a character makes it readable and grouped.

#define IOCTL_SET_VALUE _IOWR(MAGIC, 1, int)

static int major;
static int kernel_value = 0;

static long basic_ioctl(struct file *file,unsigned int cmd,unsigned long arg)
{
    int user_value;

    switch (cmd)
    {
        case IOCTL_SET_VALUE:

            // Copy value from user
            if (copy_from_user(&user_value,(int __user *)arg,sizeof(int)))
                return -EFAULT;

            printk(KERN_INFO "Received from user: %d\n", user_value);

            // Modify value inside kernel
            kernel_value = user_value + 100;

            // Send modified value back to user
            if (copy_to_user((int __user *)arg,&kernel_value,sizeof(int)))
                return -EFAULT;

            printk(KERN_INFO "Sent back to user: %d\n", kernel_value);

            break;

        default:
            return -EINVAL; //rqst is not valid
    }

    return 0;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = basic_ioctl,
};

static int __init basic_init(void)
{
    major = register_chrdev(0, DEVICE_NAME, &fops);

    if (major < 0) //Could not able to create major number
        return major;

    printk(KERN_INFO "basic_ioctl: Driver Loaded\n");
    printk(KERN_INFO "Create device node:\n");
    printk(KERN_INFO "mknod /dev/%s c %d 0\n", DEVICE_NAME, major);

    return 0;
}

static void __exit basic_exit(void)
{
    unregister_chrdev(major, DEVICE_NAME);
    printk(KERN_INFO "basic_ioctl: Driver Unloaded\n");
}

module_init(basic_init);
module_exit(basic_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Rani");
MODULE_DESCRIPTION("Basic example on IOCTL IOWR");
