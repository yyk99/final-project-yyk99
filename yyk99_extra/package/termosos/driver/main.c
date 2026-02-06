/**
 * @file main.c
 * @brief DHT11 driver functions and data
 *
 * @author Yury Y. Kuznetsov (aka yyk99)
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/printk.h>
#include <linux/types.h>
#include <linux/cdev.h>
#include <linux/fs.h> // file_operations
#include <linux/version.h>

#if LINUX_VERSION_CODE < KERNEL_VERSION(5,0,0)
#define access_ok_wrapper(type,arg,cmd) \
	access_ok(type, arg, cmd)
#else
#define access_ok_wrapper(type,arg,cmd) \
	access_ok(arg, cmd)
#endif

#include "dht11.h"
#include "dht11_driver.h"
#include "dht11_ioctl.h"

#include "bcm2835.h"

int dht11_driver_major =   0; // use dynamic major
int dht11_driver_minor =   0;

MODULE_AUTHOR("Yury Y. Kuznetsov (a.k.a yyk99)");
MODULE_LICENSE("Dual MIT/GPL");
MODULE_VERSION("1.0.0");

struct dht11_dev dht11_device;
static int gpio_pin = 4;
module_param(gpio_pin, int, S_IRUGO);
MODULE_PARM_DESC(gpio_pin, "GPIO pin number for DHT11 sensor");


int dht11_driver_open(struct inode *inode, struct file *filp)
{
	struct dht11_dev *dev;

    PDEBUG("open");

	dev = container_of(inode->i_cdev, struct dht11_dev, cdev);
	filp->private_data = dev;

    if(!mutex_trylock(&dev->lock))
        return -EBUSY;

    bcm2835_gpio_fsel(gpio_pin, BCM2835_GPIO_FSEL_INPT);
    bcm2835_delayMicroseconds(1000);

    if(dht11_get_data(&dev->dht11_self, gpio_pin)){
        strncpy(dev->text, "Inconsistent data", sizeof(dev->text));
        dev->text_size = strnlen(dev->text, sizeof(dev->text));
    } else {
        int temp_int = dev->dht11_self.temperature / 100;
        int temp_frac = abs(dev->dht11_self.temperature % 100);
        snprintf(dev->text, sizeof(dev->text),
                 "T: %d.%02dC H: %d.%02d%%\n",
                 temp_int, temp_frac,
                 dev->dht11_self.humidity / 100,
                 dev->dht11_self.humidity % 100);
        dev->text_size = strnlen(dev->text, sizeof(dev->text));
    }

    return 0;
}

int dht11_driver_release(struct inode *inode, struct file *filp)
{
	struct dht11_dev *dev;

    PDEBUG("release");

	dev = container_of(inode->i_cdev, struct dht11_dev, cdev);
	filp->private_data = dev;
    mutex_unlock(&dev->lock);

    return 0;
}

ssize_t dht11_driver_read(struct file *filp, char __user *buf, size_t count,
                loff_t *f_pos)
{
    ssize_t retval = 0;
    struct dht11_dev *dev = filp->private_data;
    loff_t offset = *f_pos;

    if (offset >= dev->text_size)
      return 0;  // EOF

    if (count > dev->text_size - offset)
        count = dev->text_size - offset;

    if (copy_to_user(buf, dev->text + offset, count)) {
        retval = -EFAULT;
        goto out;
    }

	*f_pos += count;
	retval = count;

 out:
    return retval;
}

struct file_operations dht11_fops = {
    .owner =    THIS_MODULE,
    .read =     dht11_driver_read,
    .open =     dht11_driver_open,
    .release =  dht11_driver_release,
};

static int dht11_driver_setup_cdev(struct dht11_dev *dev)
{
    int err, devno = MKDEV(dht11_driver_major, dht11_driver_minor);

    cdev_init(&dev->cdev, &dht11_fops);
    dev->cdev.owner = THIS_MODULE;
    dev->cdev.ops = &dht11_fops;
    err = cdev_add (&dev->cdev, devno, 1);
    if (err) {
        printk(KERN_ERR "Error %d adding dht11 cdev", err);
    }
    return err;
}

int dht11_driver_init_module(void)
{
    dev_t dev = 0;
    int result;
    result = alloc_chrdev_region(&dev, dht11_driver_minor, 1, "dht11_driver");
    dht11_driver_major = MAJOR(dev);
    if (result < 0) {
        printk(KERN_WARNING "Can't get major %d\n", dht11_driver_major);
        return result;
    }

    memset(&dht11_device, 0, sizeof(struct dht11_dev));
    mutex_init(&dht11_device.lock);
    result = dht11_driver_setup_cdev(&dht11_device);
    if (result) {
        goto fail;
    }

    result = bcm2835_init();
    if (result) {
        printk(KERN_ERR "Failed to initialize BCM2835\n");
        cdev_del(&dht11_device.cdev);
        goto fail;
    }
    return result;

 fail:
    unregister_chrdev_region(dev, 1);
    return result;
}

void dht11_driver_cleanup_module(void)
{
    dev_t devno = MKDEV(dht11_driver_major, dht11_driver_minor);

    // Attempt to acquire lock to ensure no active operations
    mutex_lock(&dht11_device.lock);

    bcm2835_close();

    cdev_del(&dht11_device.cdev);
    unregister_chrdev_region(devno, 1);

    mutex_unlock(&dht11_device.lock);
    mutex_destroy(&dht11_device.lock);
}

module_init(dht11_driver_init_module);
module_exit(dht11_driver_cleanup_module);
