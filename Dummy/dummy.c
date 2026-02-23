#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/fs.h>
#include<linux/init.h>
#include<linux/uaccess.h>

#define DEVICE_NAME	"dummy_char"
#define SIZE		256

static int major;
static char kern_buf[SIZE];
static int buf_size;

struct number
{
	int num1;
	int num2;
};

static int result;


static int __init dum_init(void);
static void __exit dum_exit(void);

static int dum_open(struct inode *inode, struct file *file)
{
	printk(KERN_INFO"dummy_char : device opened\n");
	return 0;
}

static int dum_close(struct inode *inode, struct file *file)
{
	printk(KERN_INFO"dummy_char : device closed\n");
	return 0;
}

static ssize_t dum_read(struct file *file, char __user *buf, size_t count, loff_t *offset)
{
	int bytes_to_copy;

	if(*offset >= buf_size)
	{
		return 0;
	}

	bytes_to_copy = min(count, (size_t) (buf_size - *offset));

	if(copy_to_user(buf, &result, sizeof(int)))
	{
		return -EFAULT;
	}

	*offset += bytes_to_copy;

	printk(KERN_INFO"dummy_char : read %d bytes\n", bytes_to_copy);

	return bytes_to_copy;
}

static ssize_t dum_write(struct file *file, const char __user *buf, size_t count, loff_t *offset)
{
	int bytes_to_copy;
	struct number nums;

	bytes_to_copy = min(count, (size_t) SIZE);

	if(copy_from_user(&nums, buf, count))
	{
		return -EFAULT;
	}
	
	result = nums.num1 + nums.num2;

	buf_size = bytes_to_copy;

	printk(KERN_INFO"dummy_char : written %d bytes\n", bytes_to_copy);

	return bytes_to_copy;
}

static struct file_operations fops = {
	.owner = THIS_MODULE,
	.open = dum_open,
	.read = dum_read,
	.write = dum_write,
	.release = dum_close
};

static int __init dum_init(void)
{
	major = register_chrdev(0, DEVICE_NAME, &fops);
	if(major < 0)
	{
		printk(KERN_ERR"dummy_char : failed to register\n");
		return major;
	}

	printk(KERN_INFO"dummy_char : loaded\n");
	printk(KERN_INFO"dummy_char : major number = %d\n", major);
	printk(KERN_INFO"Create a device node using\n");
	printk(KERN_INFO"mknod /dev/%s c %d 0\n", DEVICE_NAME, major);

	return 0;
}

static void __exit dum_exit(void)
{
	unregister_chrdev(major, DEVICE_NAME);
	printk(KERN_INFO"dummy_char : removed\n");
}

module_init(dum_init);
module_exit(dum_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Karthik");
MODULE_DESCRIPTION("Simple dummy char driver");
