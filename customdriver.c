#include  <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>

static int __init  myinit(void){
	printk(KERN_INFO "Simple driver loaded\n");
	return 0;
}
static void __exit myexit(void){
	printk(KERN_INFO "Simple driver unloaded\n");
}

module_init(myinit);
module_exit(myexit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Isshika");
MODULE_DESCRIPTION("Simple driver");

