#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/cdev.h>
#include <linux/slab.h>        // kmalloc
#include <linux/vmalloc.h>     // vmalloc
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/string.h>

#define DEVICE_NAME "memdemo"
#define KMALLOC_SIZE 128
#define VMALLOC_SIZE (8 * PAGE_SIZE)

static dev_t dev;
static struct cdev mem_cdev;

/* Buffers */
static char *kmalloc_buf;
static char *vmalloc_buf;

static ssize_t memdemo_read(struct file *file,
                            char __user *user_buf,
                            size_t count,
                            loff_t *ppos)
{
    size_t total_size = KMALLOC_SIZE + VMALLOC_SIZE;
    size_t remaining;
    size_t to_copy;

    if (*ppos >= total_size)
        return 0;

    remaining = total_size - *ppos;
    to_copy = min(count, remaining);

    /* Read from kmalloc buffer */
    if (*ppos < KMALLOC_SIZE) {

        size_t km_rem = KMALLOC_SIZE - *ppos;
        size_t km_copy = min(to_copy, km_rem);

        if (copy_to_user(user_buf,
                         kmalloc_buf + *ppos,
                         km_copy))
            return -EFAULT;

        *ppos += km_copy;
        return km_copy;
    }
    else {
        /* Read from vmalloc buffer */
        size_t offset = *ppos - KMALLOC_SIZE;

        if (copy_to_user(user_buf,
                         vmalloc_buf + offset,
                         to_copy))
            return -EFAULT;

        *ppos += to_copy;
        return to_copy;
    }
}

static const struct file_operations memdemo_fops = {
    .owner = THIS_MODULE,
    .read  = memdemo_read,
};

/* Module Init */

static int __init memdemo_init(void)
{
    int ret;

    ret = alloc_chrdev_region(&dev, 0, 1, DEVICE_NAME);
    if (ret)
        return ret;

    pr_info("memdemo: Major number = %d\n", MAJOR(dev));

    cdev_init(&mem_cdev, &memdemo_fops);

    ret = cdev_add(&mem_cdev, dev, 1);
    if (ret)
        goto unregister_dev;

    /* kmalloc: physically contiguous */
    kmalloc_buf = kmalloc(KMALLOC_SIZE, GFP_KERNEL);
    if (!kmalloc_buf) {
        ret = -ENOMEM;
        goto del_cdev;
    }

    /* vmalloc: virtually contiguous */
    vmalloc_buf = vmalloc(VMALLOC_SIZE);
    if (!vmalloc_buf) {
        ret = -ENOMEM;
        goto free_kmalloc;
    }

    /* Initialize memory */
    memset(kmalloc_buf, 'K', KMALLOC_SIZE);
    memset(vmalloc_buf, 'V', VMALLOC_SIZE);

    pr_info("memdemo: loaded (kmalloc=%d bytes, vmalloc=%lu bytes)\n",
            KMALLOC_SIZE, VMALLOC_SIZE);

    return 0;

free_kmalloc:
    kfree(kmalloc_buf);
del_cdev:
    cdev_del(&mem_cdev);
unregister_dev:
    unregister_chrdev_region(dev, 1);
    return ret;
}

static void __exit memdemo_exit(void)
{
    vfree(vmalloc_buf);
    kfree(kmalloc_buf);
    cdev_del(&mem_cdev);
    unregister_chrdev_region(dev, 1);

    pr_info("memdemo: unloaded\n");
}

module_init(memdemo_init);
module_exit(memdemo_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Narsimha");
MODULE_DESCRIPTION("Linux driver demonstration kmalloc vs vmalloc");