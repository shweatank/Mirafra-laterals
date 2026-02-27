#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/kobject.h>
#include <linux/sysfs.h>

static struct kobject *my_kobj;
static int value;

static ssize_t value_show(struct kobject *kobj,
                          struct kobj_attribute *attr,
                          char *buf)
{
    return sprintf(buf, "%d\n", value);
}

static ssize_t value_store(struct kobject *kobj,
                           struct kobj_attribute *attr,
                           const char *buf,
                           size_t count)
{
    sscanf(buf, "%d", &value);

    pr_info("sysfs value updated = %d\n", value);

    return count;
}

static struct kobj_attribute value_attr = __ATTR(value, 0664, value_show, value_store);


static int __init sysfs_demo_init(void)
{
    pr_info("SYSFS Driver Loaded\n");

    my_kobj = kobject_create_and_add("my_sysfs", kernel_kobj);

    if (!my_kobj)
        return -ENOMEM;

    sysfs_create_file(my_kobj, &value_attr.attr);

    return 0;
}

static void __exit sysfs_demo_exit(void)
{
    kobject_put(my_kobj);

    pr_info("SYSFS Driver Removed\n");
}

module_init(sysfs_demo_init);
module_exit(sysfs_demo_exit);

MODULE_LICENSE("GPL");
