#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>

#define PROC_NAME "procfs_driver"
#define BUF_SIZE 256

static char proc_buffer[BUF_SIZE] = "Hello from procfs \n";

/*Read call back*/

static ssize_t proc_read(struct file* file,
                        char __user* user_buf,
                        size_t count,
                        loff_t* ppos)
{
    return simple_read_from_buffer(user_buf, count, ppos, proc_buffer, strlen(proc_buffer));

}

/*Write call back*/

static ssize_t proc_write(struct file* file,
                            const char __user* user_buf,
                            size_t count,
                            loff_t* ppos)
{
    if(count > BUF_SIZE -1){
        return -EINVAL;
    }
    if(copy_from_user(proc_buffer, user_buf, count)){
        return -EFAULT;
    }
    
    proc_buffer[count] = '\0';
    return count;
}

static const struct proc_ops proc_file_ops = {
    .proc_read = proc_read,
    .proc_write = proc_write,
};

static int __init procfs_driver_init(void){
    proc_create(PROC_NAME, 0666, NULL, &proc_file_ops);
    pr_info("proc_demo loaded \n");
    return 1;
}

static void __exit procfs_driver_exit(void){
    remove_proc_entry(PROC_NAME, NULL);
    pr_info("proc_demo unloaded \n");
}

module_init(procfs_driver_init)
module_exit(procfs_driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Adepu Shashank");
MODULE_DESCRIPTION("Procfs driver code");

/*Use cat /proc/procfs_driver*/