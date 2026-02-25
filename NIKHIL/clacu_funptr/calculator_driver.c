#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/string.h>

#define DEVICE_NAME "calculator_driver"
#define OP_LEN 16

static int major_number;
static int result;

/* Structure sent from user */
struct calc_data {
    int a;
    int b;
    char op[OP_LEN];
};

typedef int (*op_func_t)(int, int);

/* Operations */
static int op_add(int a, int b) { return a + b; }
static int op_sub(int a, int b) { return a - b; }
static int op_mul(int a, int b) { return a * b; }
static int op_div(int a, int b)
{
    if (b == 0) {
        printk(KERN_INFO "Divide by zero error\n");
        return 0;
    }
    return a / b;
}

/* Operation table */
struct op_entry {
    const char *name;
    op_func_t func;
};

static struct op_entry op_table[] = {
    {"ADD", op_add},
    {"SUB", op_sub},
    {"MUL", op_mul},
    {"DIV", op_div},
    {NULL, NULL}
};

/* Lookup function */
static op_func_t lookup(const char *name)
{
    int i;
    for (i = 0; op_table[i].name != NULL; i++) {
        if (strcmp(op_table[i].name, name) == 0)
            return op_table[i].func;
    }
    return NULL;
}

/* Open */
static int calc_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "calculator_driver: device opened\n");
    return 0;
}

/* Release */
static int calc_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "calculator_driver: device closed\n");
    return 0;
}

/* Write */
static ssize_t calc_write(struct file *file,
                          const char __user *user_buffer,
                          size_t count,
                          loff_t *offset)
{
    struct calc_data data;
    op_func_t operation;

    if (count != sizeof(struct calc_data))
        return -EINVAL;

    if (copy_from_user(&data, user_buffer, sizeof(data)))
        return -EFAULT;

    data.op[OP_LEN - 1] = '\0';

    operation = lookup(data.op);
    if (!operation) {
        printk(KERN_INFO "Invalid operation\n");
        return -EINVAL;
    }

    result = operation(data.a, data.b);

    printk(KERN_INFO "calculator: %d %s %d = %d\n",
           data.a, data.op, data.b, result);

    return sizeof(data);
}

/* Read */
static ssize_t calc_read(struct file *file,
                         char __user *user_buffer,
                         size_t count,
                         loff_t *offset)
{
    if (*offset > 0)
        return 0;

    if (copy_to_user(user_buffer, &result, sizeof(result)))
        return -EFAULT;

    *offset = sizeof(result);
    return sizeof(result);
}

/* File operations */
static struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = calc_open,
    .release = calc_release,
    .read = calc_read,
    .write = calc_write,
};

/* Init */
static int __init calc_init(void)
{
    major_number = register_chrdev(0, DEVICE_NAME, &fops);

    if (major_number < 0) {
        printk(KERN_ALERT "Failed to register device\n");
        return major_number;
    }

    printk(KERN_INFO "calculator_driver loaded\n");
    printk(KERN_INFO "Major number = %d\n", major_number);
    printk(KERN_INFO "mknod /dev/%s c %d 0\n",
           DEVICE_NAME, major_number);

    return 0;
}

/* Exit */
static void __exit calc_exit(void)
{
    unregister_chrdev(major_number, DEVICE_NAME);
    printk(KERN_INFO "calculator_driver unloaded\n");
}

module_init(calc_init);
module_exit(calc_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Nikhil");
MODULE_DESCRIPTION("Calculator Driver using Function Pointers");
