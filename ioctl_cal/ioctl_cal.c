#include<linux/fs.h>
#include<linux/module.h>
#include<linux/uaccess.h>
#include<linux/cdev.h>
#include<linux/err.h>
#include<linux/kernel.h>


#define DRV_NAME	"calc_ioctl"
#define CLS_NAME	"calc_class"
#define DEV_NAME	"calc"

#define CALC_IOC_MAGIC 'B'

struct calc_req
{
	int num1;
	int num2;
	long res;
	int err;
};

#define CALC_IOC_ADD	_IOWR(CALC_IOC_MAGIC, 1, struct calc_req)
#define CALC_IOC_SUB	_IOWR(CALC_IOC_MAGIC, 2, struct calc_req)
#define CALC_IOC_MUL	_IOWR(CALC_IOC_MAGIC, 3, struct calc_req)
#define CALC_IOC_DIV	_IOWR(CALC_IOC_MAGIC, 4, struct calc_req)
#define CALC_IOC_MOD	_IOWR(CALC_IOC_MAGIC, 5, struct calc_req)

static dev_t g_dev;
static struct cdev g_cdev;
static struct class *g_class;
static struct device *g_device;

static long basic_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	struct calc_req cal;

	switch(cmd)
	{
		case CALC_IOC_ADD:
			if(copy_from_user(&cal, (struct calc_req __user *)arg, sizeof(struct calc_req)))
			{
				return -EFAULT;
			}
			pr_info("Kernel: received %d, %d from user\n", cal.num1, cal.num2);

			cal.res = cal.num1 + cal.num2;
			cal.err = 0;
			

			if(copy_to_user((struct calc_req __user *)arg, &cal, sizeof(struct calc_req)))
			{
				return -EFAULT;
			}
			break;
			
		case CALC_IOC_SUB:
                        if(copy_from_user(&cal, (struct calc_req __user *)arg, sizeof(struct calc_req)))
                        {
                                return -EFAULT;
                        }
                        pr_info("Kernel: received %d, %d from user\n", cal.num1, cal.num2);

                        cal.res = cal.num1 - cal.num2;
			cal.err = 0;

                        if(copy_to_user((struct calc_req __user *)arg, &cal, sizeof(struct calc_req)))
                        {
                                return -EFAULT;
                        }
                        break;

		case CALC_IOC_MUL:
                        if(copy_from_user(&cal, (struct calc_req __user *)arg, sizeof(struct calc_req)))
                        {
                                return -EFAULT;
                        }
                        pr_info("Kernel: received %d, %d from user\n", cal.num1, cal.num2);

                        cal.res = cal.num1 * cal.num2;
			cal.err = 0;

                        if(copy_to_user((struct calc_req __user *)arg, &cal, sizeof(struct calc_req)))
                        {
                                return -EFAULT;
                        }
                        break;

		case CALC_IOC_DIV:
                        if(copy_from_user(&cal, (struct calc_req __user *)arg, sizeof(struct calc_req)))
                        {
                                return -EFAULT;
                        }
                        pr_info("Kernel: received %d, %d from user\n", cal.num1, cal.num2);

			if(cal.num2 == 0)
			{
				return -EINVAL;
			}

                        cal.res = cal.num1 / cal.num2;
			cal.err = 0;

                        if(copy_to_user((struct calc_req __user *)arg, &cal, sizeof(struct calc_req)))
                        {
                                return -EFAULT;
                        }
                        break;

		case CALC_IOC_MOD:
                        if(copy_from_user(&cal, (struct calc_req __user *)arg, sizeof(struct calc_req)))
                        {
                                return -EFAULT;
                        }
                        pr_info("Kernel: received %d, %d from user\n", cal.num1, cal.num2);

                        if(cal.num2 == 0)
			{
				return -EINVAL;
			}

			cal.res = cal.num1 % cal.num2;
			cal.err = 0;

                        if(copy_to_user((struct calc_req __user *)arg, &cal, sizeof(struct calc_req)))
                        {
                                return -EFAULT;
                        }
                        break;



		default:
			return -EINVAL;
	}
	return 0;
}

static struct file_operations fops = {
	.owner		= THIS_MODULE,
	.unlocked_ioctl	= basic_ioctl,
};

#if 0
static int __init basic_init(void)
{
	major = register_chrdev(0, DEVICE_NAME , &fops);
	pr_info("calc_ioctl: loaded, major=%d\n", major);
	return 0;
}

static void __exit basic_exit(void)
{
	unregister_chrdev(major, DEVICE_NAME);
	pr_info("calc_ioctl: unloaded\n");
}
#endif

static int __init calc_init(void)
{
	int ret;

	ret = alloc_chrdev_region(&g_dev, 0, 1, DRV_NAME);
	
	if(ret)
	{
		pr_err(DRV_NAME ": alloc_chr_dev_region failed: %d\n", ret);
		return ret;
	}

	cdev_init(&g_cdev, &fops);
	//g_dev.owner = THIS_MODULE;

	ret = cdev_add(&g_cdev, g_dev, 1);

	if(ret)
	{
		pr_err(DRV_NAME ": cdev_add failed: %d\n", ret);
		goto unregister;
	}
	
	g_class = class_create(CLS_NAME);
	
	if(IS_ERR(g_class))
	{
		ret = PTR_ERR(g_class);
		pr_err(DRV_NAME ": class_create failed: %d\n", ret);
		goto device;
	}

	g_device = device_create(g_class, NULL, g_dev, NULL, DEV_NAME);
	if(IS_ERR(g_device))
	{
		ret = PTR_ERR(g_device);
		pr_err(DRV_NAME ": device_create failed: %d\n", ret);
		goto class;
	}

	pr_info(DRV_NAME ": loaded, /dev/%s major = %d minor = %d\n", DEV_NAME, MAJOR(g_dev), MINOR(g_dev));

	return 0;

class:
	class_destroy(g_class);
device:
	cdev_del(&g_cdev);
unregister:
	unregister_chrdev_region(g_dev, 1);
	return ret;
}

static void __exit calc_exit(void)
{
	device_destroy(g_class, g_dev);
	class_destroy(g_class);
	cdev_del(&g_cdev);
	unregister_chrdev_region(g_dev, 1);

	pr_info(DRV_NAME ": unloaded\n");
}


module_init(calc_init);
module_exit(calc_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("KARTHIK");
MODULE_DESCRIPTION("Calculator ioctl char driver");
