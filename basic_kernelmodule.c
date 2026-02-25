#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "Basic_Char"
#define BUF_SIZE 128

static int major_number;
static char ker_buf[BUF_SIZE];
static int buf_size;

static int basic_open( struct inode *inode, struct file *fp)
{
	printk(KERN_INFO "Basic_char : device open\n");
	return 0;
}

static int basic_release( struct inode *inode, struct file *fp)
{
	printk(KERN_INFO "Basic_char : device closed\n");
        return 0;
}


static ssize_t basic_read(struct file *file,
                          char __user *user_buf,
                          size_t count,
                          loff_t *offset)
{
	int bytes_to_copy;
	if( *offset >= buf_size)
		return 0;

	bytes_to_copy = min( count, ( size_t)( buf_size - *offset));

	if( copy_to_user( user_buf, ker_buf + *offset, bytes_to_copy))
		return -EFAULT;

	*offset += bytes_to_copy;

	printk(KERN_INFO "Basic_char : read %d bytes\n", bytes_to_copy);

	return bytes_to_copy;
}


/*
static ssize_t basic_read(struct file *file,
                          char __user *user_buf,
                          size_t count,
                          loff_t *offset)
{
    if (*offset >= buf_size)
        return 0;

    if (copy_to_user(user_buf, ker_buf, buf_size))
        return -EFAULT;

    *offset += buf_size;

    return buf_size;
}


static ssize_t basic_write(struct file *file,
                           const char __user *user_buf,
                           size_t count,
                           loff_t *offset)
{
    int nums[2];
    int result;

    if (copy_from_user(nums, user_buf, sizeof(nums)))
        return -EFAULT;

    result = nums[0] + nums[1];

    memcpy(ker_buf, &result, sizeof(result));
    buf_size = sizeof(result);

    printk(KERN_INFO "Received: %d %d, Result: %d\n",
            nums[0], nums[1], result);

    return count;
}
*/
/*
static ssize_t basic_write(struct file *file,
                           const char __user *user_buf,
                           size_t count,
                           loff_t *offset)
{
	int bytes_to_copy;
	bytes_to_copy = min( count, (size_t)BUF_SIZE);

	if( copy_from_user( ker_buf, user_buf, bytes_to_copy))
		return -EFAULT;

	buf_size = bytes_to_copy;

        printk(KERN_INFO "Basic_char : wrote %d bytes\n", bytes_to_copy);

        return bytes_to_copy;
}
*/
static ssize_t basic_write(struct file *file,
                           const char __user *user_buf,
                           size_t count,
                           loff_t *offset)
{
    size_t bytes_to_copy;

    memset(ker_buf, 0, BUF_SIZE);

    bytes_to_copy = min(count, (size_t)(BUF_SIZE - 1));

    if (copy_from_user(ker_buf, user_buf, bytes_to_copy))
        return -EFAULT;

    ker_buf[bytes_to_copy] = '\0';  // null terminate
    buf_size = bytes_to_copy;

    printk(KERN_INFO "Written: %s\n", ker_buf);

    return bytes_to_copy;
}

static struct file_operations basic_fops = {
	.owner = THIS_MODULE,
	.open = basic_open,
	.read = basic_read,
	.write = basic_write,
	.release = basic_release,
};

static int __init basic_char_init( void)
{
	major_number = register_chrdev( 0, DEVICE_NAME, &basic_fops);

	if( major_number < 0)
	{
		printk( KERN_INFO "Basic_char: Failed to register device\n");
		return major_number;
	}

	printk( KERN_INFO "Basic_char: Loaded\n");
	printk( KERN_INFO "Basic_char: major_number = %d\n", major_number);
	printk( KERN_INFO "Create device node with:\n");
	printk( KERN_INFO "mknod /dev/%s c %d 0\n", DEVICE_NAME, major_number);

	return 0;
}


static void __exit basic_char_exit(void)
{
	unregister_chrdev( major_number, DEVICE_NAME);
    	printk(KERN_INFO "Basic_char: Kernel Module Unloaded\n");
}

module_init( basic_char_init);
module_exit( basic_char_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Rani");
MODULE_DESCRIPTION("basic char driver");
