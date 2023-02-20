/**************************************************************
* Class:  CSC-415-0# Fall 2021
* Name: Donnovan Jiles
* Student ID: 920975689
* GitHub UserID: DonnovanJiles70122
* Project: Assignment 6 – Device Driver
*
* File: <device_driver.c>
*
* Description: Create a simple device driver
*
**************************************************************/

#include <linux/cdev.h>
#include <linux/device.h>
#include<linux/slab.h>                 
#include<linux/uaccess.h>              
#include <linux/ioctl.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>

#define NAME_OF_DEVICE "dev_device"  // the location will be at /dev/dev_driver

#define WR_VALUE _IOW('a','a',int32_t*)   // ioctl with write param. 
#define RD_VALUE _IOR('a','b',int32_t*)   // ioctl with read param.

int32_t value = 0;
dev_t dev = 0;

static struct class *dev_class;     
static struct device *dev_device;   
static int major_num;               // number that indicates the device

// device driver prototype functions 

static int __init dev_driver_init(void);
static void __exit dev_driver_exit(void);
static int dev_open(struct inode *inode, struct file *file);
static int dev_release(struct inode *inode, struct file *file);
static ssize_t dev_read(struct file *filp, char __user *buf, size_t len,loff_t * off);
static ssize_t dev_write(struct file *filp, const char *buf, size_t len, loff_t * off);
static long dev_ioctl(struct file *file, unsigned int cmd, unsigned long arg);

// map functionality to file operations from functions above

static struct file_operations fops =
{
    .owner          = THIS_MODULE,
    .read           = dev_read,
    .write          = dev_write,
    .open           = dev_open,
    .unlocked_ioctl = dev_ioctl,
    .release        = dev_release,
};

// 
static int dev_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "Device is open\n");
    return 0;
}

static int dev_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "Device is closed\n");
    return 0;
}


static ssize_t dev_read(struct file *filp, char __user *buf, size_t len, loff_t *off)
{
   int count = 0;

   count = copy_to_user((int32_t*) buf, &value, sizeof(value));

   if(count == 0){
      printk(KERN_INFO "sent results to user\n");
   } else {
      printk(KERN_INFO "Failed to send results\n");
      return -1;
   }
    printk(KERN_INFO "Read function\n");
    return 0;
}
static ssize_t dev_write(struct file *filp, const char __user *buf, size_t len, loff_t *off)
{
   copy_from_user(&value, (int32_t*)buf, sizeof(value));
   printk(KERN_INFO "VALUE = %d\n",value);
   printk(KERN_INFO "Write function\n");
   return 0;
}


// take user writes and send user reads
static long dev_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    switch(cmd) {
        case WR_VALUE:
            copy_from_user(&value ,(int32_t*) arg, sizeof(value));
            printk(KERN_INFO "Value = %d\n", value);
            break;
        case RD_VALUE:
            copy_to_user((int32_t*) arg, &value, sizeof(value));
            printk(KERN_INFO "Value successfully sent\n");
            break;
    }
    return 0;
}

/* this function initializes the device driver
*/
static int __init dev_driver_init(void)
{
   printk(KERN_INFO "Initializing the Device Driver\n");


   // For registering the device for the system
   major_num = register_chrdev(0, "dev_device", &fops);

   // Error handling 
   if(major_num < 0){
      printk(KERN_INFO "major number register failed\n");
      return -1;
   }

   printk(KERN_INFO "major number(%d) registered", major_num);

   // creation to register the device class

   dev_class = class_create(THIS_MODULE, "dev_class");

   if(IS_ERR(dev_class)){
      unregister_chrdev(major_num, NAME_OF_DEVICE);
      printk(KERN_ALERT "Device class register failed\n");
      return PTR_ERR(dev_class);
   }

   printk(KERN_INFO "Device class is registered\n");

   // register driver for our device

   dev_device = device_create(dev_class, NULL, MKDEV(major_num, 0), NULL, NAME_OF_DEVICE);

   if(IS_ERR(dev_device)){
      class_destroy(dev_class);
      unregister_chrdev(major_num, NAME_OF_DEVICE);
      printk(KERN_ALERT "Device creation Failed\n");
      return PTR_ERR(dev_device);
   }

   printk(KERN_INFO "Device has been created\n");

   return 0;

}

// unregister 

void __exit dev_driver_exit(void)
{
    device_destroy(dev_class, MKDEV(major_num, 0));
    class_destroy(dev_class);
    class_unregister(dev_class);
    unregister_chrdev(major_num, NAME_OF_DEVICE);
    printk(KERN_INFO "Driver was removed\n");
}

module_init(dev_driver_init);
module_exit(dev_driver_exit);

MODULE_LICENSE("GPL");                  // license for this driver
MODULE_AUTHOR("Donnovan Jiles");     
MODULE_DESCRIPTION("A device driver");
MODULE_VERSION("1.5");