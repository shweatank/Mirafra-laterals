#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

static int __init hello_module_init(void)
{
    pr_info("Hello World from Module - Insertion successful\n");
    return 0;
}

static void __exit hello_module_exit(void)
{
    pr_info("Good bye from Module - Removal successful\n");
}

module_init(hello_module_init);
module_exit(hello_module_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Adepu Shashank");
MODULE_DESCRIPTION("Our first dynamically loadable kernel module");
