//
// Created by 蓝色空间 on 2022/9/1.
//

#include "device.h"

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/device.h>

#include "ioctl.h"

static int major = 0;

static int spy_open(struct inode *pnode, struct file *spy_file) {
     pr_info("[kern func]: %s  major: %d  minor: %d\n", __FUNCTION__, imajor(pnode), iminor(pnode));
     return 0;
}

static ssize_t spy_read(struct file *spy_file, char __user *buf, size_t count, loff_t *offptr) {
    unsigned char ary[100] = "you are reading successfully!";
    unsigned long len = min(count, sizeof(ary));
    int retval;
    pr_info("[kern func]: %s  major: %d  minor: %d\n", __FUNCTION__, imajor(spy_file->f_inode), iminor(spy_file->f_inode));
     if(copy_to_user(buf, ary, len) != 0){
           retval = -EFAULT;
           goto cp_err;
     }
     return len;

cp_err:
     return retval;
}

static ssize_t spy_write(struct file *spy_file, const char __user *buf, size_t count, loff_t *offptr) {
    unsigned char ary[100] = "";
    unsigned long len = min(count, sizeof(ary));
    int retval;
    pr_info("[spy_device] > %s  major: %d  minor: %d\n", __FUNCTION__, imajor(spy_file->f_inode), iminor(spy_file->f_inode));
    if(copy_from_user(ary, buf, len) != 0){
           retval = -EFAULT;
           goto cp_err;
    }
    printk("[spy_device] > writing context: %s\n",ary);
    return len;

cp_err:
    return retval;
}

long spy_ioctl (struct file *spy_file, unsigned int ioctl, unsigned long arg) {
    long err;
    switch (ioctl) {
        case SPY_ENABLE:
            err = hv_enable((struct spy_system __user *)arg);
            break;
        case SPY_DISABLE:
            err = hv_disable();
            break;
        case SPY_TARGET_CREATE:
            err = hv_target_create((struct spy_target_create __user *)arg);
            break;
        case SPY_TARGET_LOAD:
            err = hv_target_load((struct spy_target_load __user *)arg);
            break;
        case SPY_TARGET_START:
            err = hv_target_start((const char __user *)arg);
            break;
        case SPY_TARGET_ELIMINATE:
            err = hv_target_eliminate((const char __user *)arg);
            break;
        default:
            err = -EINVAL;
            break;
    }
    return err;
}

static int spy_release(struct inode *pnode, struct file *spy_file) {
     pr_info("[kern func]: %s  major: %d  minor: %d\n", __FUNCTION__, imajor(pnode), iminor(pnode));
     return 0;
}

static struct file_operations fops = {
      .owner = THIS_MODULE,
      .read = spy_read,
      .write = spy_write,
      .open = spy_open,
      .release = spy_release,
}; 

static struct class *spy_class;

int spy_device_init(void) {
    struct device *spy_device;
    int i;
    int retval;
 
    pr_info("[spy_device] > this is a driver spy, in module initial function\n");
    major = register_chrdev(0, "spy_chrdev", &fops);
    if(major < 0){
            retval = major;
            goto chrdev_err;
    }
    
    spy_class = class_create(THIS_MODULE,"spy_class");
    if(IS_ERR(spy_class)){
           retval =  PTR_ERR(spy_class);
           goto class_err;
    }

    if (DEVICE_COUNT > 1) {
        for (i = 0; i < DEVICE_COUNT; i++) {
            spy_device = device_create(spy_class, NULL, MKDEV(major, i), NULL, "spy%d", i);
            if (IS_ERR(spy_device)) {
                retval = PTR_ERR(spy_device);
                goto device_err;
            }
        }
    }
    else {
        spy_device = device_create(spy_class, NULL, MKDEV(major, i), NULL, "spy");
        if (IS_ERR(spy_device)) {
            retval = PTR_ERR(spy_device);
            goto device_err;
        }
    }
    return 0;

device_err: 
    while(i--) 
         class_destroy(spy_class); 
class_err: 
    unregister_chrdev(major, "spy_chrdev");
chrdev_err: 
    return retval; 
}

void spy_device_exit(void) {
      int i;
      pr_info("[spy_device] > in module exit function\n");
      unregister_chrdev(major, "spy_chrdev");
      for(i=0; i < DEVICE_COUNT; i++) {
          device_destroy(spy_class, MKDEV(major, i));
      }
      class_destroy(spy_class);
}