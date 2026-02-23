#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>          
#include <linux/device.h>        

#define DEVICE_NAME "calc_ioctl_class"
#define CALC_IOC_MAGIC 'B'
#define DRV_NAME "calc_device"
#define CLASS_NAME "calc_class"  

struct calc_req {
    int a;
    int b;
    long result;
    int err;
};

static dev_t g_dev;
static struct cdev g_cdev;
static struct class  *g_class;
static struct device *g_device;

#define CALC_IOC_ADD _IOWR(CALC_IOC_MAGIC, 1, struct calc_req)
#define CALC_IOC_SUB _IOWR(CALC_IOC_MAGIC, 2, struct calc_req)
#define CALC_IOC_MUL _IOWR(CALC_IOC_MAGIC, 3, struct calc_req)
#define CALC_IOC_DIV _IOWR(CALC_IOC_MAGIC, 4, struct calc_req)
#define CALC_IOC_MOD _IOWR(CALC_IOC_MAGIC, 5, struct calc_req)

static inline bool calc_ioctl_valid(unsigned int cmd){
    return _IOC_TYPE(cmd) == CALC_IOC_MAGIC;
}

static long calc_ioctl(struct file *file, unsigned int cmd, unsigned long arg);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Adepu Shashank");
MODULE_DESCRIPTION("Calculator IOCTL module");

static struct file_operations calc_fops = {
    .owner          = THIS_MODULE,
    .unlocked_ioctl = calc_ioctl,
};

static int __init calc_init(void){   
    int ret;

    ret = alloc_chrdev_region(&g_dev, 0, 1, DRV_NAME);
    if(ret){
        pr_err(DRV_NAME ": alloc_chrdev_region failed : %d\n", ret);
        return ret;                  
    }

    cdev_init(&g_cdev, &calc_fops);
    g_cdev.owner = THIS_MODULE;

    ret = cdev_add(&g_cdev, g_dev, 1);
    if(ret){
        pr_err(DRV_NAME ": cdev_add failed : %d\n", ret);
        unregister_chrdev_region(g_dev, 1);
        return ret;
    }

    g_class = class_create(CLASS_NAME);
    if(IS_ERR(g_class)){
        ret = PTR_ERR(g_class);
        pr_err(DRV_NAME " : class_create failed %d\n", ret);
        cdev_del(&g_cdev);
        unregister_chrdev_region(g_dev, 1);
        return ret;
    }

    g_device = device_create(g_class, NULL, g_dev, NULL, DEVICE_NAME);
    if(IS_ERR(g_device)){
        ret = PTR_ERR(g_device);
        pr_err(DRV_NAME ": device_create failed : %d\n", ret);  
        class_destroy(g_class);
        cdev_del(&g_cdev);
        unregister_chrdev_region(g_dev, 1);
        return ret;
    }

    pr_info(DRV_NAME " loaded /dev/%s major = %d minor = %d\n",
            DEVICE_NAME, MAJOR(g_dev), MINOR(g_dev));  
    return 0;
}

static long calc_ioctl(struct file *file,
                        unsigned int cmd,
                        unsigned long arg)
{
    struct calc_req user_req;


    if(copy_from_user(&user_req, (struct calc_req __user *)arg, sizeof(struct calc_req))){ 
    }

    user_req.err = 0;

    switch(cmd){
        case CALC_IOC_ADD:
            user_req.result = (long)user_req.a + user_req.b;
            break;

        case CALC_IOC_SUB:
            user_req.result = (long)user_req.a - user_req.b;
            break;

        case CALC_IOC_MUL:
            user_req.result = (long)user_req.a * user_req.b;
            break;

        case CALC_IOC_DIV:
            if(user_req.b == 0){
                user_req.err = -EINVAL;
                user_req.result = 0;
                break;
            }
            user_req.result = (long)user_req.a / user_req.b;
            break;

        case CALC_IOC_MOD:
            if(user_req.b == 0){
                user_req.err = -EINVAL;
                user_req.result = 0;
                break;
            }
            user_req.result = (long)user_req.a % user_req.b;
            break;

        default:
            return -EINVAL;
    }

    pr_info("Calc result is %ld err no is %d\n", user_req.result, user_req.err); 

    if(copy_to_user((struct calc_req __user *)arg, &user_req, sizeof(struct calc_req)))
        return -EFAULT;

    return 0;
}

static void __exit calc_exit(void){   
    device_destroy(g_class, g_dev);
    class_destroy(g_class);
    cdev_del(&g_cdev);
    unregister_chrdev_region(g_dev, 1);
    pr_info(DRV_NAME " unloaded\n");
}

module_init(calc_init);  
module_exit(calc_exit);