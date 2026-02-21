#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

static int __init sugandh_entry(void)
{
    printk(KERN_INFO "Hello: Module Loaded into Kernel\n");
    return 0;
}

static void __exit sugandh_exit(void)
{
    printk(KERN_INFO "Hello: Module Removed from Kernel\n");
}



module_init(sugandh_entry);
module_exit(sugandh_exit);


MODULE_LIECENSE("GPL");
MODULE_AUTHOR("SUGANDH");
