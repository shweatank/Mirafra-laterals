#include<linux/init.h>
#include<linux/module.h>

static int __init my_int(void)
{
    printk(KERN_INFO "Module loaded");
    return 0;
}
static void __exit my_exit(void)
{
    printk(KERN_INFO "Module unloaded");
}

module_init(my_int);
module_exit(my_exit);
MODULE_LICENSE("GPL");