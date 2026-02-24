#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/init.h>

MODULE_LICENSE("GPL");
//MODULE_INFO("LDD");
MODULE_AUTHOR("MIRAFRA");

static int __init mod_init(void)
{
	int num1 = 5, num2 = 0;
	int res = num1 / num2;
	printk(KERN_INFO"result = %d\n", res);
	printk(KERN_INFO"Module loaded\n");
	return 0;
}

static void __exit mod_exit(void)
{
	printk(KERN_INFO"Module removed\n");
}

module_init(mod_init);
module_exit(mod_exit);
