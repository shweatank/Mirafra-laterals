#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <asm/uaccess.h>
#include <linux/string.h>

#define DEVICE_NAME "reverse_char_dev"
#define BUFFER_SIZE 256
#define CLASS_NAME "reverse_class"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Nikhil");
MODULE_DESCRIPTION("Character Driver to Reverse Strings");
MODULE_VERSION("1.0");

static int majorNumber;
static struct class *reverseClass;
static struct device *reverseDevice;
static char kernelBuffer[BUFFER_SIZE] = {0};
static char reversedBuffer[BUFFER_SIZE] = {0};
static size_t bufferSize = 0;

/* Function to reverse a string */
static void reverseString(char *str, char *rev, size_t len)
{
    size_t i;
    for (i = 0; i < len; i++) {
        rev[i] = str[len - 1 - i];
    }
    rev[len] = '\0';
}

/* File operations: open */
static int fileOpen(struct inode *inodep, struct file *filep)
{
    printk(KERN_INFO "reverse_driver: Device opened\n");
    return 0;
}

/* File operations: close/release */
static int fileRelease(struct inode *inodep, struct file *filep)
{
    printk(KERN_INFO "reverse_driver: Device closed\n");
    return 0;
}

/* File operations: read */
static ssize_t fileRead(struct file *filep, char __user *buffer, size_t len, loff_t *offset)
{
    int error_count = 0;
    
    /* Reverse the kernel buffer */
    if (bufferSize > 0) {
        reverseString(kernelBuffer, reversedBuffer, bufferSize);
    }
    
    /* Send reversed string to user space */
    error_count = copy_to_user(buffer, reversedBuffer, bufferSize);
    
    if (error_count == 0) {
        printk(KERN_INFO "reverse_driver: Sent %zu bytes to user space\n", bufferSize);
        size_t bytesReturned = bufferSize;
        memset(kernelBuffer, 0, BUFFER_SIZE);
        memset(reversedBuffer, 0, BUFFER_SIZE);
        bufferSize = 0;
        return bytesReturned;
    } else {
        printk(KERN_ERR "reverse_driver: Failed to send %d bytes to user space\n", error_count);
        return -EFAULT;
    }
}

/* File operations: write */
static ssize_t fileWrite(struct file *filep, const char __user *buffer, size_t len, loff_t *offset)
{
    int error_count = 0;
    
    /* Check if adding new data would exceed buffer size */
    if (bufferSize + len > BUFFER_SIZE - 1) {
        len = BUFFER_SIZE - 1 - bufferSize;
    }
    
    /* Append data from user space to kernel space (don't overwrite!) */
    error_count = copy_from_user(&kernelBuffer[bufferSize], buffer, len);
    
    if (error_count == 0) {
        bufferSize += len;  /* Accumulate size, don't overwrite */
        kernelBuffer[bufferSize] = '\0';
        printk(KERN_INFO "reverse_driver: Received %zu bytes from user space: %s\n", len, kernelBuffer);
        return len;
    } else {
        printk(KERN_ERR "reverse_driver: Failed to receive %d bytes from user space\n", error_count);
        return -EFAULT;
    }
}

/* File operations structure */
static struct file_operations fops =
{
    .open = fileOpen,
    .read = fileRead,
    .write = fileWrite,
    .release = fileRelease,
};

/* Module initialization */
static int __init reverseDriver_init(void)
{
    printk(KERN_INFO "reverse_driver: Initializing the Reverse Character Driver\n");
    
    /* Allocate a device number */
    majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
    if (majorNumber < 0) {
        printk(KERN_ERR "reverse_driver: Failed to register a major number\n");
        return majorNumber;
    }
    printk(KERN_INFO "reverse_driver: Registered successfully with major number %d\n", majorNumber);
    
    /* Register device class */
    reverseClass = class_create(CLASS_NAME);
    if (IS_ERR(reverseClass)) {
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ERR "reverse_driver: Failed to register device class\n");
        return PTR_ERR(reverseClass);
    }
    printk(KERN_INFO "reverse_driver: Device class registered successfully\n");
    
    /* Create device file */
    reverseDevice = device_create(reverseClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
    if (IS_ERR(reverseDevice)) {
        class_destroy(reverseClass);
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ERR "reverse_driver: Failed to create device file\n");
        return PTR_ERR(reverseDevice);
    }
    printk(KERN_INFO "reverse_driver: Device file created successfully\n");
    
    return 0;
}

/* Module cleanup */
static void __exit reverseDriver_exit(void)
{
    device_destroy(reverseClass, MKDEV(majorNumber, 0));
    class_unregister(reverseClass);
    class_destroy(reverseClass);
    unregister_chrdev(majorNumber, DEVICE_NAME);
    printk(KERN_INFO "reverse_driver: Goodbye from the Reverse Character Driver\n");
}

module_init(reverseDriver_init);
module_exit(reverseDriver_exit);
