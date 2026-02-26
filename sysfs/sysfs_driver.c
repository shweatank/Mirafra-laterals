#include <linux/module.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>

static struct kobject *demo_kobj;
static int demo_value;

static ssize_t value_show(struct kobject *kobj,
                           struct kobj_attribute *attr,
                           char *buf)
{
    return sprintf(buf, "%d\n", demo_value);
}

static ssize_t value_store(struct kobject *kobj,
                            struct kobj_attribute *attr,
                            const char *buf,
                            size_t count)
{
    sscanf(buf, "%d", &demo_value);
    return count;
}

static struct kobj_attribute value_attr = __ATTR(value, 0664, value_show, value_store);

static int __init my_sysfs_init(void)
{
    demo_kobj = kobject_create_and_add("sysfs_demo", kernel_kobj);

    if (!demo_kobj)
        return -ENOMEM;

    sysfs_create_file(demo_kobj, &value_attr.attr);
    pr_info("sysfs_driver loaded\n");
    return 0;
}

static void __exit my_sysfs_exit(void)
{
    kobject_put(demo_kobj);
    pr_info("sysfs_driver unloaded\n");
}

module_init(my_sysfs_init);
module_exit(my_sysfs_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Adepu Shashank");
MODULE_DESCRIPTION("Sysfs driver code");

/*
sudo insmod sysfs_driver
cat /sys/kernel/sysfs_demo/value
0
echo 42 | sudo tee /sys/kernel/sysfs_demo/value 
42
cat /sys/kernel/sysfs_demo/value 
42
sudo rmmod sysfs_driver */