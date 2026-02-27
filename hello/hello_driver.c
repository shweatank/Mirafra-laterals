#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>


// Module init
static int __init hello_world_init(void)
{
    pr_info("Hello World LDD File\n");

    return 0;
}

// Module exit
static void __exit hello_world_exit(void)
{
    pr_info("Hello world module Exited\n");
}

module_init(hello_world_init);
module_exit(hello_world_exit);

MODULE_VERSION("V1.0.0");
MODULE_DESCRIPTION("First LDD File");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("SHARAN");
