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
MODULE_VERSION("0.5.1");

struct dht11_dev dht11_device;
static int bit_RPI_BPLUS_GPIO_J8_07     =  4;  /*!< B+, Pin J8-07 */

int dht11_driver_open(struct inode *inode, struct file *filp)
{
	struct dht11_dev *dev;

    PDEBUG("open");

	dev = container_of(inode->i_cdev, struct dht11_dev, cdev);
	filp->private_data = dev;

    if(!mutex_trylock(&dev->lock))
        return -EBUSY;

    bcm2835_gpio_fsel(RPI_BPLUS_GPIO_J8_07, BCM2835_GPIO_FSEL_INPT);
    bcm2835_delayMicroseconds(1000);

    if(dht11_get_data(&dev->dht11_self, bit_RPI_BPLUS_GPIO_J8_07)){
        strncpy(dev->text, "Inconsistent data", sizeof(dev->text));
        dev->text_size = strnlen(dev->text, sizeof(dev->text));
    } else {
        snprintf(dev->text, sizeof(dev->text),
                 "T: %d.%02dC H: %d.%02d%%\n",
                 dev->dht11_self.temperature / 100,
                 dev->dht11_self.temperature % 100,
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

long dht11_driver_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    int err = 0;
    int retval = 0;
    struct dht11_seekto req;
    struct dht11_dev *dev = filp->private_data;

    PDEBUG("dht11_driver_ioctl: cmd %u, arg %lu", cmd, arg);
	/*
	 * extract the type and number bitfields, and don't decode
	 * wrong cmds: return ENOTTY (inappropriate ioctl) before access_ok()
	 */
	if (_IOC_TYPE(cmd) != DHT11_IOC_MAGIC)
        return -ENOTTY;
	if (_IOC_NR(cmd) > DHT11CHAR_IOC_MAXNR)
        return -ENOTTY;

	/*
	 * the direction is a bitmask, and VERIFY_WRITE catches R/W
	 * transfers. `Type' is user-oriented, while
	 * access_ok is kernel-oriented, so the concept of "read" and
	 * "write" is reversed
	 */
	if (_IOC_DIR(cmd) & _IOC_READ)
		err = !access_ok_wrapper(VERIFY_WRITE, (void __user *)arg, _IOC_SIZE(cmd));
	else if (_IOC_DIR(cmd) & _IOC_WRITE)
		err =  !access_ok_wrapper(VERIFY_READ, (void __user *)arg, _IOC_SIZE(cmd));
	if (err)
        return -EFAULT;

	switch(cmd) {
    case DHT11CHAR_IOCSEEKTO:
        if(copy_from_user(&req, (struct dht11_seekto __user *)arg, sizeof(struct dht11_seekto)))
            return -EFAULT;

        if(mutex_lock_interruptible(&dev->lock))
            return -ERESTARTSYS;

        PDEBUG("dht11_driver_ioctl: DHT11CHAR_IOCSEEKTO: {%u %d}", req.cmd, req.cmd_aux);

        break;
    default:  /* redundant, as cmd was checked against MAXNR */
		return -ENOTTY;
	}
	return retval;
}

ssize_t dht11_driver_read(struct file *filp, char __user *buf, size_t count,
                loff_t *f_pos)
{
    ssize_t retval = 0;
    struct dht11_dev *dev = filp->private_data;
    loff_t offset = *f_pos;

    if (count > dev->text_size - offset)
        count = dev->text_size - offset;

    if (copy_to_user(buf, dev->text + offset, count)) {
        retval = -EFAULT;
        goto out;
    }

	*f_pos += count;
	retval = count;

 out:
    mutex_unlock(&dev->lock);
    return retval;
}

struct file_operations dht11_fops = {
    .owner =    THIS_MODULE,
    .read =     dht11_driver_read,
    .unlocked_ioctl = dht11_driver_ioctl,
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
    result = dht11_driver_setup_cdev(&dht11_device);
    if( result ) {
        goto fail;
    }

    result = bcm2835_init();
    if (result)
	goto fail;

    return result;

 fail:
    unregister_chrdev_region(dev, 1);
    return result;
}

void dht11_driver_cleanup_module(void)
{
    dev_t devno = MKDEV(dht11_driver_major, dht11_driver_minor);

    bcm2835_close();

    cdev_del(&dht11_device.cdev);
    unregister_chrdev_region(devno, 1);
}

module_init(dht11_driver_init_module);
module_exit(dht11_driver_cleanup_module);
