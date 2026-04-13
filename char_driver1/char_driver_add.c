#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "basic_char_add"
#define BUFFER_SIZE 256

static int major_number;
static int numbers[2];
static int result;


static int basic_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "basic_char: driver open\n");
    return 0;
}

/* Close */
static int basic_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "basic_char: device close\n");
    return 0;
}
/* Read */
static ssize_t basic_read(struct file *file,
                          char __user *user_buffer,
                          size_t count,
                          loff_t *offset)
{

    if (*offset > 0)
        return 0;
    
    if (count < sizeof(result))
        return -EINVAL;

    if (copy_to_user(user_buffer,
                     &result,
                     sizeof(result)))
        return -EFAULT;
    
    *offset = sizeof(result);
    printk(KERN_INFO "In basic read result is %d\n",result);
    return sizeof(result);


}

/* Write */
static ssize_t basic_write(struct file *file,
                           const char __user *user_buffer,
                           size_t count,
                           loff_t *offset)
{
    if(count < sizeof(numbers)){
        return -EFAULT;
    }

    if(copy_from_user(numbers, user_buffer, sizeof(numbers))){
        return -EFAULT;
    }

    result = numbers[0] * numbers[1];
    printk(KERN_INFO "In basic_write  result is %d\n",result);
    return sizeof(numbers);

}

/* File Operations Structure */
static struct file_operations basic_fops = {
    .owner   = THIS_MODULE,
    .open    = basic_open,
    .read    = basic_read,
    .write   = basic_write,
    .release = basic_release,
};

/* Init */
static int __init basic_char_init(void)
{
    major_number = register_chrdev(0, DEVICE_NAME, &basic_fops);

    if (major_number < 0) {
        printk(KERN_ALERT "basic_char : failed to register device\n");
        return major_number;
    }

    printk(KERN_INFO "basic_char : loaded\n");
    printk(KERN_INFO "basic_char : major number = %d\n", major_number);
    printk(KERN_INFO "Create device node using:\n");
    printk(KERN_INFO "mknod /dev/%s c %d 0\n", DEVICE_NAME, major_number);

    return 0;
}

/* Exit */
static void __exit basic_char_exit(void)
{
    unregister_chrdev(major_number, DEVICE_NAME);
    printk(KERN_INFO "basic_char : unloaded\n");
}

module_init(basic_char_init);
module_exit(basic_char_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("SHARANG");
MODULE_DESCRIPTION("Basic Character Device Driver");
