#include<linux/module.h>
#include<linux/fs.h>
#include<linux/uaccess.h>
#include<linux/cdev.h>

#define DEVICE_NAME "calci_ioctl"
#define MAGIC 'a'
#define CLASS_NAME  "calci_class"

struct calc_req{
    int a;
    int b;
    long result;
    int error;
};

#define CALC_IOC_ADD _IOWR(MAGIC, 1, struct calc_req)
#define CALC_IOC_SUB _IOWR(MAGIC, 2, struct calc_req)
#define CALC_IOC_MUL _IOWR(MAGIC, 3, struct calc_req)
#define CALC_IOC_DIV _IOWR(MAGIC, 4, struct calc_req)
#define CALC_IOC_MOD _IOWR(MAGIC, 5, struct calc_req)

static dev_t g_dev;
static struct cdev g_cdev;
static struct class *g_class;
static struct device *g_device;

static long calci_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    struct calc_req request;

    if (copy_from_user(&request, (struct calc_req __user *)arg, sizeof(request)))
        return -EFAULT;
    request.error = 0;

    switch(cmd){
        case CALC_IOC_ADD:request.result = request.a + request.b;
			  break;
        case CALC_IOC_SUB:request.result = request.a - request.b;
			  break;
        case CALC_IOC_MUL:request.result = request.a * request.b;
			  break;
        case CALC_IOC_DIV:request.result = request.a / request.b;
			  break;
        case CALC_IOC_MOD:request.result = request.a % request.b;
			  break;
       	default:
			  return -EINVAL;
    }

    if(copy_to_user((struct calc_req __user *)arg, &request, sizeof(request)))
        return -EFAULT;
    return 0;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = calci_ioctl,
};

/*static int __init calc_init(void)
{
    major = register_chrdev(0, DEVICE_NAME, &fops);
    if (major < 0)
        return major;

    printk(KERN_INFO "Calci driver loaded\n");
    printk(KERN_INFO "Create device node:\n");
    printk(KERN_INFO "mknod /dev/%s c %d 0\n", DEVICE_NAME, major);

    return 0;
}*/


static int __init calc_init(void)
{
	int ret;
	ret=alloc_chrdev_region(&g_dev, 0, 1, DEVICE_NAME);
	if(ret)
	{
		pr_err(DEVICE_NAME ": alloc_chrdev_regin failed: %d\n", ret);
		return ret;
	}

	cdev_init(&g_cdev, &fops); //initalize cdev
	g_cdev.owner = THIS_MODULE;

	ret = cdev_add(&g_cdev, g_dev, 1);
	if(ret)
	{
		pr_err(DEVICE_NAME ":  cdev_add failed: %d\n", ret);
		unregister_chrdev_region(g_dev, 1);
		return ret;
	}
	//create class
	g_class = class_create(CLASS_NAME);
	
	if(IS_ERR(g_class))
	{
		ret=PTR_ERR(g_class);
		pr_err(DEVICE_NAME ":class_create failed: %d\n",ret);
		cdev_del(&g_cdev);
		unregister_chrdev_region(g_dev, 1);
		return ret;
	}
	//create device
	g_device=device_create(g_class, NULL, g_dev, NULL, DEVICE_NAME);

	if(IS_ERR(g_device))
	{
		ret=PTR_ERR(g_device);
		pr_err(DEVICE_NAME ": device_create failed: %d\n", ret);
		class_destroy(g_class);
		cdev_del(&g_cdev);
		unregister_chrdev_region(g_dev, 1);
                return ret;
	}

	pr_info(DEVICE_NAME ":loaded. /dev/%s major=%d minor=%d\n", DEVICE_NAME, MAJOR(g_dev), MINOR(g_dev));
	return 0;
}

static void __exit calc_exit(void)
{
    device_destroy(g_class, g_dev);
    class_destroy(g_class);
    cdev_del(&g_cdev);
    unregister_chrdev_region(g_dev,1);
    printk(KERN_INFO "Calci driver unloaded\n");
}

module_init(calc_init);
module_exit(calc_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Rani");
MODULE_DESCRIPTION("IOCTL Calculator Driver");
