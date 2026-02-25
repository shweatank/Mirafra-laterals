#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sysfs.h>
#include <linux/kobject.h>
#include <linux/init.h>

static struct kobject *demo_kobj;
static int demo_value;

/* Show function */
static ssize_t value_show(struct kobject *kobj,
                          struct kobj_attribute *attr,
                          char *buf)
{
    return sprintf(buf, "%d\n", demo_value);
}

/* Store function */
static ssize_t value_store(struct kobject *kobj,
                           struct kobj_attribute *attr,
                           const char *buf,
                           size_t count)
{
    sscanf(buf, "%d", &demo_value);
    return count;   // MUST return count
}

/* Attribute structure */
static struct kobj_attribute value_attr =
    __ATTR(value, 0664, value_show, value_store);

/* Module Init */
static int __init sysfs_demo_init(void)
{
    int ret;

    demo_kobj = kobject_create_and_add("sysfs_demo", kernel_kobj);
    if (!demo_kobj)
        return -ENOMEM;

    ret = sysfs_create_file(demo_kobj, &value_attr.attr);
    if (ret) {
        pr_err("Failed to create sysfs file\n");
        kobject_put(demo_kobj);
        return ret;
    }

    pr_info("sysfs_demo loaded\n");
    return 0;
}

/* Module Exit */
static void __exit sysfs_demo_exit(void)
{
    sysfs_remove_file(demo_kobj, &value_attr.attr);
    kobject_put(demo_kobj);
    pr_info("sysfs_demo unloaded\n");
}

module_init(sysfs_demo_init);
module_exit(sysfs_demo_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Narsimha");
MODULE_DESCRIPTION("Simple sysfs driver");



/*
sudo insmod sysfs_driver
cat /sys/kernel/sysfs_demo/value
0
echo 42 | sudo tee /sys/kernel/sysfs_demo/value 
42
cat /sys/kernel/sysfs_demo/value 
42
sudo rmmod sysfs_driver */