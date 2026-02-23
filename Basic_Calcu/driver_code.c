#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "basic_char"

#define OP_SUM 1
#define OP_SUB 2
#define OP_MUL 3
#define OP_DIV 4

static int major_number;

/* Structure to cpoy*/
struct data{
    int ops;
    int a;
    int b;
    int result;
};

static struct data kdata;

/* ----------- Function Pointer Section ----------- */

typedef (*operatins)(int,int);

static int add(int a, int b)
{
    return a + b;
}

static int sub(int a, int b)
{
    return a - b;
}

static int mul(int a, int b)
{
    return a * b;
}

static int divide(int a, int b)
{
    if (b == 0)
        return -1;
    return a / b;
}

/* Function pointer table */
static operatins op_table[] = {add,sub,mul,divide};

/* ----------- File Operations ----------- */

static int basic_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "basic_char: opened\n");
    return 0;
}

static int basic_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "basic_char: closed\n");
    return 0;
}

static ssize_t basic_write(struct file *file,
                           const char __user *userbuff,
                           size_t count,
                           loff_t *offset)
{
    if (copy_from_user(&kdata, userbuff, sizeof(kdata)))
        return -EFAULT;

    if (kdata.op > 0 &&
        kdata.op < ARRAY_SIZE(op_table) &&
        op_table[kdata.op] != NULL)
    {
        kdata.result = op_table[kdata.op](kdata.a, kdata.b);
    }
    else
    {
        kdata.result = -1;
    }

    printk(KERN_INFO "basic_char: operation done, result=%d\n",
           kdata.result);

    return sizeof(kdata);
}

static ssize_t basic_read(struct file *file,
                          char __user *user_buffer,
                          size_t count,
                          loff_t *offset)
{
    if (*offset > 0)
        return 0;

    if (copy_to_user(user_buffer, &kdata, sizeof(kdata)))
        return -EFAULT;

    *offset += sizeof(kdata);

    return sizeof(kdata);
}

/* File operations structure */
static struct file_operations basic_fops = {
    .owner   = THIS_MODULE,
    .open    = basic_open,
    .read    = basic_read,
    .write   = basic_write,
    .release = basic_release,
};

/* ----------- Init & Exit ----------- */

static int __init basic_char_init(void)
{
    major_number = register_chrdev(0, DEVICE_NAME, &basic_fops);

    if (major_number < 0) {
        printk(KERN_ALERT "basic_char: registration failed\n");
        return major_number;
    }

    printk(KERN_INFO "basic_char: loaded\n");
    printk(KERN_INFO "basic_char: major number = %d\n", major_number);
    printk(KERN_INFO "Create device:\n");
    printk(KERN_INFO "mknod /dev/%s c %d 0\n",
           DEVICE_NAME, major_number);

    return 0;
}

static void __exit basic_char_exit(void)
{
    unregister_chrdev(major_number, DEVICE_NAME);
    printk(KERN_INFO "basic_char: unloaded\n");
}

module_init(basic_char_init);
module_exit(basic_char_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Naren");
MODULE_DESCRIPTION("Char Driver with Function Pointer Calculator");
MODULE_VERSION("1.0");