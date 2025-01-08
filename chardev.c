/**
 * @file chardev.c
 * @brief A simple character device driver implementation.
 *
 * This module implements a character device that can be read from and written to from userspace.
 * It demonstrates the basic operations of a Linux kernel module.
 *
 * @author Mohamed Gebril
 * @version 1.0
 * @date 2025-01-08
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "chardev"
#define CLASS_NAME "char_class"

/* Buffer for storing data */
static char device_buffer[256];
static size_t buffer_pointer = 0;

/* Device number variables */
static dev_t dev_num;
static struct class *device_class;
static struct cdev char_device;

/* Function prototypes */
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_read(struct file *, char __user *, size_t, loff_t *);
static ssize_t device_write(struct file *, const char __user *, size_t, loff_t *);

/**
 * @brief File operations structure.
 */
static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = device_read,
    .write = device_write,
    .open = device_open,
    .release = device_release
};

/**
 * @brief Open function for the character device.
 * @param inode Pointer to the inode structure.
 * @param file Pointer to the file structure.
 * @return 0 on success.
 */
static int device_open(struct inode *inode, struct file *file)
{
    try_module_get(THIS_MODULE);
    return 0;
}

/**
 * @brief Release function for the character device.
 * @param inode Pointer to the inode structure.
 * @param file Pointer to the file structure.
 * @return 0 on success.
 */
static int device_release(struct inode *inode, struct file *file)
{
    module_put(THIS_MODULE);
    return 0;
}

/**
 * @brief Read function for the character device.
 * @param filp Pointer to the file structure.
 * @param buffer Pointer to the user buffer.
 * @param length Number of bytes to read.
 * @param offset Current offset.
 * @return Number of bytes read.
 */
static ssize_t device_read(struct file *filp, char __user *buffer, size_t length, loff_t *offset)
{
    int bytes_read = 0;
    
    if (*offset >= buffer_pointer)
        return 0;
    
    bytes_read = min(length, buffer_pointer - (size_t)*offset);
    
    if (copy_to_user(buffer, device_buffer + *offset, bytes_read))
        return -EFAULT;
    
    *offset += bytes_read;
    return bytes_read;
}

/**
 * @brief Write function for the character device.
 * @param filp Pointer to the file structure.
 * @param buffer Pointer to the user buffer.
 * @param length Number of bytes to write.
 * @param offset Current offset.
 * @return Number of bytes written.
 */
static ssize_t device_write(struct file *filp, const char __user *buffer, size_t length, loff_t *offset)
{
    if (length >= sizeof(device_buffer))
        return -EINVAL;
        
    if (copy_from_user(device_buffer, buffer, length))
        return -EFAULT;
        
    buffer_pointer = length;
    return length;
}

/**
 * @brief Module initialization function.
 * @return 0 on success.
 */
static int __init chardev_init(void)
{
    int ret;
    struct device *dev_ret;

    /* Allocate a major number dynamically */
    ret = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
    if (ret < 0) {
        printk(KERN_ALERT "Failed to allocate major number\n");
        return ret;
    }

    /* Create the device class */
    device_class = class_create(CLASS_NAME);
    if (IS_ERR(device_class)) {
        unregister_chrdev_region(dev_num, 1);
        printk(KERN_ALERT "Failed to create device class\n");
        return PTR_ERR(device_class);
    }

    /* Create the device */
    dev_ret = device_create(device_class, NULL, dev_num, NULL, DEVICE_NAME);
    if (IS_ERR(dev_ret)) {
        class_destroy(device_class);
        unregister_chrdev_region(dev_num, 1);
        printk(KERN_ALERT "Failed to create device\n");
        return PTR_ERR(dev_ret);
    }

    /* Initialize the char device structure */
    cdev_init(&char_device, &fops);
    char_device.owner = THIS_MODULE;

    /* Add the device to the system */
    ret = cdev_add(&char_device, dev_num, 1);
    if (ret < 0) {
        device_destroy(device_class, dev_num);
        class_destroy(device_class);
        unregister_chrdev_region(dev_num, 1);
        printk(KERN_ALERT "Failed to add device to system\n");
        return ret;
    }

    printk(KERN_INFO "Character device driver loaded successfully\n");
    return 0;
}

/**
 * @brief Module cleanup function.
 */
static void __exit chardev_exit(void)
{
    cdev_del(&char_device);
    device_destroy(device_class, dev_num);
    class_destroy(device_class);
    unregister_chrdev_region(dev_num, 1);
    printk(KERN_INFO "Character device driver unloaded successfully\n");
}

module_init(chardev_init);
module_exit(chardev_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Mohamed Gebril");
MODULE_DESCRIPTION("A simple character device driver");
MODULE_VERSION("1.0");
