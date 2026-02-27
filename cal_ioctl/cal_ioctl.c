#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "calc_ioctl"
#define CALC_IOC_MAGIC 'B'

#define DRV_NAME "calc_device"

struct calc_req {
    int a;
    int b;
    long result;
    int err;
};

static int major;
dev_t g_dev;

/*each ioctl copies struct calc_req in/out*/
/*user fills a,b : driver fills result, err*/

#define CALC_IOC_ADD _IOWR(CALC_IOC_MAGIC, 1, struct calc_req)
#define CALC_IOC_SUB _IOWR(CALC_IOC_MAGIC, 2, struct calc_req)
#define CALC_IOC_MUL _IOWR(CALC_IOC_MAGIC, 3, struct calc_req)
#define CALC_IOC_DIV _IOWR(CALC_IOC_MAGIC, 4, struct calc_req)
#define CALC_IOC_MOD _IOWR(CALC_IOC_MAGIC, 5, struct calc_req)

/* optional : validate ioctl type*/
static inline bool calc_ioctl_valid(unsigned int cmd){
    return _IOC_TYPE(cmd) == CALC_IOC_MAGIC;
}

static long calc_ioctl(struct file *file, unsigned int cmd, unsigned long arg);


MODULE_LICENSE("GPL");
MODULE_AUTHOR("SHARANG");
MODULE_DESCRIPTION("Calculator ioctl  module");

static struct file_operations calc_fops = {
    .owner              = THIS_MODULE,
    .unlocked_ioctl     = calc_ioctl,
};



static long calc_ioctl(struct file* file,
                        unsigned int cmd,
                        unsigned long arg)
{
    struct calc_req user_req;
    if(copy_from_user(&user_req, (struct call_req __user*)arg, sizeof(struct calc_req))){
        return -EFAULT;
    }

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
            if (user_req.b == 0) {
                user_req.err = -EINVAL;   
                user_req.result = 0;
                break;
            }
            user_req.result = (long)user_req.a / user_req.b;
            break;

        case CALC_IOC_MOD:
            if (user_req.b == 0) {
                user_req.err = -EINVAL;
                user_req.result = 0;
                break;
            }
            user_req.result = (long)user_req.a % user_req.b;
            break;

        default:
            return -EINVAL;
    }

    pr_info("Calc result is %d err no is %d\n",user_req.result, user_req.err);

    if (copy_to_user((struct calc_req __user *)arg, &user_req, sizeof(struct calc_req)))
        return -EFAULT;

    return 0;

}


static int __init basic_init(void){
    major = register_chrdev(0, DEVICE_NAME, &calc_fops);
    pr_info("Calc ioctl loaded , major number = %d\n", major);
    return 0;
}


static void __exit basic_exit(void){
    unregister_chrdev(major, DEVICE_NAME);
    pr_info("Calc ioctl unloaded \n");
}


module_init(basic_init);    
module_exit(basic_exit);

