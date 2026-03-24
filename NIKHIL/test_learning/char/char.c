#include<linux/module.h>
#include<linux/init.h>

static int __init my_int(void)
{
printk(KERN_INFO "Hello Kernel\n");    return 0;
}

static void __exit my_exit(void)
{
    printk(KERN_INFO "Hello module unloade");
}

module_init(my_int);
module_exit(my_exit);
MODULE_LICENSE("GPL");


