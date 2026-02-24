#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/cdev.h>
#include <linux/device.h>

#define DEVICE_NAME "calc_dev"
#define CALC_IOC_MAGIC 'C'

struct calc_req{
    int a;
    int b;
    long result;
    int err;
};

static int major;
//Each ioctl copies struct calc_req in/out.
//use fills a,b; driver fills rsult, err.

#define CALC_IOC_ADD _IOWR(CALC_IOC_MAGIC, 1, struct  calc_req)
#define CALC_IOC_SUB _IOWR(CALC_IOC_MAGIC, 2, struct  calc_req)
#define CALC_IOC_MUL _IOWR(CALC_IOC_MAGIC, 3, struct  calc_req)
#define CALC_IOC_DIV _IOWR(CALC_IOC_MAGIC, 4, struct  calc_req)
#define CALC_IOC_MOD _IOWR(CALC_IOC_MAGIC, 5, struct  calc_req)


//Optional : Validate ioctl type
static inline bool calc_ioctl_valid(unsigned int cmd)
{
    return _IOC_TYPE(cmd) ==  CALC_IOC_MAGIC;
}

//----- char device plumbing --------
static dev_t g_dev;
static struct cdev g_cdev;
static struct class *g_class;


static long calc_ioctl(struct file *file,
                        unsigned int cmd,
                        unsigned long arg)
{
    if (!calc_ioctl_valid(cmd))  
         return -EINVAL;

    struct calc_req req;
    if (copy_from_user(&req,
                       (struct calc_req __user *)arg,
                       sizeof(req)))
        return -EFAULT;

        req.err =0 ;
        
    switch (cmd)
    {
       case CALC_IOC_ADD:
        req.result =req.a + req.b;
        break;

    case CALC_IOC_SUB:
        req.result =req.a - req.b;
        break;

     case CALC_IOC_MUL:
        /* code */
        req.result =req.a * req.b;
        break;

    case CALC_IOC_DIV:
    if (req.b == 0) {
        req.err = -1;
    } else {
        req.result = req.a / req.b;
    }
    break;
    
    case CALC_IOC_MOD:
        if (req.b == 0) {
            req.err = -1;
        } else {
            req.result = req.a % req.b;
        }
        break;

    
    default:
      return  -EINVAL;
    }
pr_info("RESULT is %d\n", req.result);
if(copy_to_user((struct calc_req __user *)arg,
                     &req,
                     sizeof(req))){
        return -EFAULT;}
 return 0;
}
static struct file_operations calc_fops = {
    .owner = THIS_MODULE,
    .unlocked_ioctl = calc_ioctl,
};


static int __init basic_init(void)
{
    major = register_chrdev(0,DEVICE_NAME,&calc_fops);
    pr_info("Calculator driver loaded,major = %d\n",major);
    return 0;
}

static void __exit basic_exit(void)
{
    unregister_chrdev(major,DEVICE_NAME);
    pr_info("Calculator driver unloaded\n");
}
module_init(basic_init);
module_exit(basic_exit);
MODULE_LICENSE("GPL");

