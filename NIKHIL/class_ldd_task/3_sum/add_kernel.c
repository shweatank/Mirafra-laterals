#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/minmax.h>  

#define DEVICE_NAME "add_kernel"
#define BUFFER_SIZE 256

static int major_number;
static char kernel_buffer[BUFFER_SIZE];
static int buffer_size = 0;
static int result;

/* Called when device is opened */
static int basic_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "basadd_kernelic_char: device opened\n");
    return 0;
}

/* Called when device is closed */
static int basic_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "add_kernel: device closed\n");
    return 0;
}

/* Called when user reads from /dev/basic_char */
static ssize_t basic_read(struct file *file,
                          char __user *user_buffer,
                          size_t count,
                          loff_t *offset)
{
    int bytes_to_copy;

    if (*offset > 0)
        return 0;   // EOF

    if (copy_to_user(user_buffer,
                     &result,
                     sizeof(result)))
        return -EFAULT;
    
    *offset = sizeof(result);


    printk(KERN_INFO "add_kernel: read %d bytes\n", sizeof(result));

    return sizeof(result);
}

/* Called when user writes to /dev/basic_char */
static ssize_t basic_write(struct file *file,const char __user *user_buffer,
                           size_t count,loff_t *offset)
{
    int numbers[2];
    if (count < sizeof(numbers))
        return -EINVAL;
    if (copy_from_user(numbers, user_buffer, sizeof(numbers)))
        return -EFAULT;
    result = numbers[0] + numbers[1];
printk(KERN_INFO "result is %d total\n", result);

    
    return sizeof(numbers);
}

/* File operations structure */
static struct file_operations basic_fops = {
    .owner   = THIS_MODULE,
    .open    = basic_open,
    .read    = basic_read,
    .write   = basic_write,
    .release = basic_release,
};

/* Module initialization */
static int __init basic_char_init(void)
{
    major_number = register_chrdev(0, DEVICE_NAME, &basic_fops);

    if (major_number < 0) {
        printk(KERN_ALERT "add_kernel: failed to register device\n");
        return major_number;
    }

    printk(KERN_INFO "add_kernel: loaded successfully\n");
    printk(KERN_INFO "add_kernel: major number = %d\n", major_number);
    printk(KERN_INFO "Create device node using:\n");
    printk(KERN_INFO "mknod /dev/%s c %d 0\n",
           DEVICE_NAME, major_number);

    return 0;
}

/* Module cleanup */
static void __exit basic_char_exit(void)
{
    unregister_chrdev(major_number, DEVICE_NAME);
    printk(KERN_INFO "add_kernel: unloaded\n");
}

/* Register init and exit functions */
module_init(basic_char_init);
module_exit(basic_char_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Nikhil");
MODULE_DESCRIPTION("Educational add_kernel character driver with file operations");