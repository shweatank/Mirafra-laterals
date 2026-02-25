#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Nikhil");
MODULE_DESCRIPTION("Very Simple Linux Driver");
MODULE_VERSION("1.0");

/* Init Function */
static int __init simple_init(void)
{
    printk(KERN_INFO "You are into the init function\n");
    int a=4;
    a=a/0;
    printk(KERN_INFO "Simple Driver: Init function called\n");
    return 0;   // return 0 means success
}

/* Exit Function */
static void __exit simple_exit(void)
{
    printk(KERN_INFO "Simple Driver: Exit function called\n");
}

module_init(simple_init);
module_exit(simple_exit);