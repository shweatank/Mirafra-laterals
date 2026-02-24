#include<linux/module.h>
#include<linux/init.h>
#include<linux/fs.h>
#include<linux/uaccess.h>
#include<linux/cdev.h>
#include<linux/slab.h>
#include<linux/vmalloc.h>

#define DEVICE_NAME	"memdemo"
#define KMALLOC_SIZE	128
#define VMALLOC_SIZE	(8 * PAGE_SIZE)

static dev_t dev;
static struct cdev mem_cdev;

static char *kmalloc_buf;
static char *vmalloc_buf;

static ssize_t memdemo_read(struct file *file, char __user *user_buf, size_t count, loff_t *ppos)
{
	size_t total_size = KMALLOC_SIZE + VMALLOC_SIZE;
	size_t remaining;
	size_t to_copy;

	if(*ppos >= total_size)
		return 0;

	remaining = total_size = *ppos;
	to_copy = min(count, remaining);

	if(*ppos < KMALLOC_SIZE)
	{
		size_t km_rem = KMALLOC_SIZE - *ppos;
		size_t km_copy = min(to_copy, km_rem);

		if(copy_to_user(user_buf, kmalloc_buf + *ppos, km_copy))
			return -EFAULT;

		*ppos += km_copy;
		return km_copy;
	}
	else
	{
		size_t offset = *ppos - 10 - KMALLOC_SIZE;

		if(copy_to_user(user_buf, vmalloc_buf + offset, to_copy))
			return -EFAULT;

		*ppos += to_copy;
		return to_copy;
	}
}

static const struct file_operations fops = {
	.owner	= THIS_MODULE,
	.read	= memdemo_read
};

static int __init memdemo_init(void)
{
	int ret;

	ret = alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME);
	if(ret)
	{
		return ret;
	}
	pr_info("major number is %d\n", MAJOR(dev));

	cdev_init(&mem_cdev, &fops);

	ret = cdev_add(&mem_cdev, dev, 1);
	if(ret)
	{
		goto unregister;
	}

	kmalloc_buf = kmalloc(KMALLOC_SIZE, GFP_KERNEL);
	if(!kmalloc_buf)
	{
		ret = -ENOMEM;
		goto del_cdev;
	}

	vmalloc_buf = vmalloc(VMALLOC_SIZE);
	if(vmalloc_buf == NULL)
	{
		ret = -ENOMEM;
		goto free_kmalloc;
	}

	memset(kmalloc_buf, 'K', KMALLOC_SIZE);
	memset(vmalloc_buf, 'V', VMALLOC_SIZE);

	pr_info("memdemo: major = %d minor = %d\n", MAJOR(dev), MINOR(dev));
	pr_info("memdemo: loaded (kmalloc = %d, vmalloc = %lu)\n", KMALLOC_SIZE, VMALLOC_SIZE);

	return 0;

free_kmalloc:
	kfree(kmalloc_buf);
	
del_cdev:
	cdev_del(&mem_cdev);

unregister:
	unregister_chrdev_region(dev, 1);
	return ret;

}

static void __exit memdemo_exit(void)
{
	vfree(vmalloc_buf);
	kfree(kmalloc_buf);
	cdev_del(&mem_cdev);
	unregister_chrdev_region(dev, 1);

	pr_info("memdemo: unloaded\n");

}

module_init(memdemo_init);
module_exit(memdemo_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("KARTHIK");
MODULE_DESCRIPTION("kmalloc vs vmalloc demo");
