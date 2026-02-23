#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "function_pointer_in_driver"
#define OP_LEN 16

static int major_number;
static int numbers[2];
static int result;

typedef int (*op_func_t)(int a, int b);

static int op_add(int a, int b){
    return a + b;
}

static int op_sub(int a, int b){
    return a - b;
}

static int op_mul(int a, int b){
    return a * b;
}

static int op_div(int a, int b){
    if(b == 0){
        printk(KERN_INFO "Cant divide by 0 , SIGFPE\n");
        return 0;
    }
    return a / b;
}

struct op_entry{
    const char* name;
    op_func_t op_func;
};

static struct op_entry op_table[] = {
    {"ADD", op_add},
    {"SUB", op_sub},
    {"MUL", op_mul},
    {"DIV", op_div},
    {NULL, NULL},
};

static op_func_t lookup(const char* name){
    for(int i = 0; op_table[i].name != NULL; i++){
        if(strcmp(op_table[i].name, name) == 0){
            return op_table[i].op_func;
        }
    }

    return NULL;

}





/* Read */
static ssize_t basic_read(struct file *file,
                          char __user *user_buffer,
                          size_t count,
                          loff_t *offset)
{

    if (*offset > 0)
        return 0;
    
    if (count < sizeof(result))
        return -EINVAL;

    if (copy_to_user(user_buffer,
                     &result,
                     sizeof(result)))
        return -EFAULT;
    
    *offset = sizeof(result);
    printk(KERN_INFO "In  read result is %d\n",result);
    return sizeof(result);


}

/* Write */
static ssize_t basic_write(struct file *file,
                           const char __user *user_buffer,
                           size_t count,
                           loff_t *offset)
{
    op_func_t op;
    char operation[OP_LEN];
    if(count < sizeof(numbers)){
        return -EFAULT;
    }

    if(copy_from_user(numbers, user_buffer, sizeof(numbers))){
        return -EFAULT;
    }

    if(copy_from_user(operation, user_buffer + 2 * sizeof(int), OP_LEN)){
        return -EFAULT;
    }
    operation[OP_LEN - 1] = '\0';

    op = lookup(operation);
    if(!op){
        printk(KERN_INFO "Invalid operation\n");
        return 0;
    }

    result = op(numbers[0], numbers[1]);
    printk(KERN_INFO " fun_pointer: %d %s %d = %d\n",
           numbers[0], operation, numbers[1], result);


    
    return 2 * sizeof(int) + OP_LEN;

}

/* File Operations Structure */
static struct file_operations basic_fops = {
    .owner   = THIS_MODULE,
    .read    = basic_read,
    .write   = basic_write,
};

/* Init */
static int __init basic_char_init(void)
{
    major_number = register_chrdev(0, DEVICE_NAME, &basic_fops);

    if (major_number < 0) {
        printk(KERN_ALERT "function_pointer_in_driver : failed to register device\n");
        return major_number;
    }

    printk(KERN_INFO "function_pointer_in_driver : loaded\n");
    printk(KERN_INFO "function_pointer_in_driver : major number = %d\n", major_number);
    printk(KERN_INFO "Create device node using:\n");
    printk(KERN_INFO "mknod /dev/%s c %d 0\n", DEVICE_NAME, major_number);

    return 0;
}

/* Exit */
static void __exit basic_char_exit(void)
{
    unregister_chrdev(major_number, DEVICE_NAME);
    printk(KERN_INFO "function_pointer_in_driver : unloaded\n");
}

module_init(basic_char_init);
module_exit(basic_char_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("SHASHANK");
MODULE_DESCRIPTION("Function Pointer Device Driver");
