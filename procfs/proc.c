#include<linux/module.h>
#include<linux/proc_fs.h>
#include<linux/uaccess.h>
//#include<string.h>


#define PROC_NAME	"proc_demo"
#define BUF_SIZE	128

static char proc_buf[BUF_SIZE] = "Hello from procfs";

static ssize_t proc_read(struct file *file, char __user *user_buf, size_t count, loff_t *ppos)
{
	return simple_read_from_buffer(user_buf, count, ppos, proc_buf, strlen(proc_buf));
}

static ssize_t proc_write(struct file *file, const char __user *user_buf, size_t count, loff_t *ppos)
{
	if(count > BUF_SIZE - 1)
	{
		return -EINVAL;
	}

	if(copy_from_user(proc_buf, user_buf, count))
	{
		return -EFAULT;
	}

	proc_buf[count] = '\0';

	return count;
}

static const struct proc_ops proc_fops = {
	.proc_read	= proc_read,
	.proc_write	= proc_write
};

static int __init proc_demo_init(void)
{
	proc_create(PROC_NAME, 0666, NULL, &proc_fops);
	pr_info("proc_demo: loaded\n");

	return 0;
}

static void __exit proc_demo_exit(void)
{
	remove_proc_entry(PROC_NAME, NULL);
	pr_info("proc_demo: unloaded\n");
}

module_init(proc_demo_init);
module_exit(proc_demo_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Karthik");
MODULE_DESCRIPTION("Simple procfs driver");
