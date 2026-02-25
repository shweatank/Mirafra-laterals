#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

static int __init divzero_init(void)
{
    int a = 10;
    int b = 0;
    int result;

    pr_info("Divide-by-zero module loaded\n");
    result = a / b;

    pr_info("Result: %d\n", result);

    return 0;
}

static void __exit divzero_exit(void)
{
    pr_info("Divide-by-zero module unloaded\n");
}

module_init(divzero_init);
module_exit(divzero_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Rani");
MODULE_DESCRIPTION("Kernel module to demonstrate divide by zero exception");
