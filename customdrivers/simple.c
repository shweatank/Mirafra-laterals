#include <linux/module.h>
#include <linux/kernel.h>

/* Module init function */
static int __init simple_init(void)
{
    pr_info("Simple module loaded\n");
    return 0;
}

/* Module exit function */
static void __exit simple_exit(void)
{
    pr_info("Simple module unloaded\n");
}

module_init(simple_init);
module_exit(simple_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("Simple Init and Exit Module");
MODULE_VERSION("1.0");
