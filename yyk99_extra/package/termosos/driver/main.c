/**
 * @file main.c
 * @brief DHT11 driver functions and data related to the
 *
 * Based on the implementation of the "scull" device driver, found in
 * Linux Device Drivers example code.
 *
 * @author yyk99
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
#include "dht11_ioctl.h"

int dht11_driver_major =   0; // use dynamic major
int dht11_driver_minor =   0;

MODULE_AUTHOR("Yury K. (a.k.a yyk99)");
MODULE_LICENSE("Dual MIT/GPL");

struct dht11_dev dht11_device;

int dht11_driver_open(struct inode *inode, struct file *filp)
{
	struct dht11_dev *dev; /* device information */

    PDEBUG("open");

	dev = container_of(inode->i_cdev, struct dht11_dev, cdev);
	filp->private_data = dev; /* for other methods */

    return 0;
}

int dht11_driver_release(struct inode *inode, struct file *filp)
{
    PDEBUG("release");
    return 0;
}

/*
 * The ioctl() implementation
 */

long dht11_driver_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	int err = 0;
	int retval = 0;
    ssize_t off;
    struct aesd_seekto req;
    struct dht11_dev *dev = filp->private_data;

    (void)dev;
    (void)req;
    (void)off;

    PDEBUG("dht11_driver_ioctl: cmd %u, arg %lu", cmd, arg);
	/*
	 * extract the type and number bitfields, and don't decode
	 * wrong cmds: return ENOTTY (inappropriate ioctl) before access_ok()
	 */
	if (_IOC_TYPE(cmd) != AESD_IOC_MAGIC)
        return -ENOTTY;
	if (_IOC_NR(cmd) > AESDCHAR_IOC_MAXNR)
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
#if 0
    case AESDCHAR_IOCSEEKTO:
        if(copy_from_user(&req, (struct aesd_seekto __user *)arg, sizeof(struct aesd_seekto)))
            return -EFAULT;

        if(mutex_lock_interruptible(&dev->lock))
            return -ERESTARTSYS;

        off = aesd_circular_buffer_offset(dev->lines, req.write_cmd, req.write_cmd_offset);
        mutex_unlock(&dev->lock);

        PDEBUG("aesd_ioctl: AESDCHAR_IOCSEEKTO: {%u %d} => %ld", req.write_cmd, req.write_cmd_offset, (long int)off);

        if (off == -1)
            return -EINVAL;
        filp->f_pos = off;
        break;
#endif
    default:  /* redundant, as cmd was checked against MAXNR */
		return -ENOTTY;
	}
	return retval;
}

/*
 * The "extended" operations -- only seek
 */
#if 0
loff_t dht11_driver_llseek(struct file *filp, loff_t off, int whence)
{
	struct dht11_dev *dev = filp->private_data;
	loff_t newpos;

    PDEBUG("dht11_driver_llseek: whence %d, offset %lld", whence, off);

	switch(whence) {
    case 0: /* SEEK_SET */
		newpos = off;
		break;

    case 1: /* SEEK_CUR */
		newpos = filp->f_pos + off;
		break;

    case 2: /* SEEK_END */
        if(mutex_lock_interruptible(&dev->lock))
            return -ERESTARTSYS;

		newpos = aesd_circular_buffer_size(dev->lines) + off;
        mutex_unlock(&dev->lock);
		break;

    default: /* can't happen */
		return -EINVAL;
	}
	if (newpos < 0)
        return -EINVAL;
	filp->f_pos = newpos;
	return newpos;
}
#endif

ssize_t dht11_driver_read(struct file *filp, char __user *buf, size_t count,
                loff_t *f_pos)
{
    ssize_t retval = 0;
    struct dht11_dev *dev = filp->private_data;
    size_t offset;

    (void)offset;

    PDEBUG("read %zu bytes with offset %lld", count, *f_pos);

    count = 0; /* FIX: DEBUG */

#if 0
	if (mutex_lock_interruptible(&dev->lock))
		return -ERESTARTSYS;

    ep = aesd_circular_buffer_find_entry_offset_for_fpos(dev->lines, *f_pos, &offset);
    if (!ep) {
        retval = 0;
        goto out;
    }
    if (offset > ep->size) { /* ASSERT */
        printk(KERN_ERR "%s: offset = %zu is > size = %zu\n", __func__, offset, ep->size);
        retval = -EFAULT;
        goto out;
    }
	if (count > ep->size - offset)
		count = ep->size - offset;

	if (copy_to_user(buf, ep->buffptr + offset, count)) {
		retval = -EFAULT;
		goto out;
	}
#endif
	*f_pos += count;
	retval = count;

 /* out: */
    mutex_unlock(&dev->lock);
    return retval;
}

ssize_t dht11_driver_write(struct file *filp, const char __user *buf, size_t count,
                loff_t *f_pos)
{
    struct dht11_dev *dev = filp->private_data;
	ssize_t retval = -ENOMEM;

    PDEBUG("write %zu bytes with offset %lld", count, *f_pos);

	if (mutex_lock_interruptible(&dev->lock))
		return -ERESTARTSYS;

#if 0
    tmp_size = dev->line_buffer_size + count;
    tmp_buf = krealloc(dev->line_buffer, tmp_size, GFP_KERNEL);
    if(!tmp_buf) {
        retval = -ENOMEM;
        goto out;
    }

    dev->line_buffer = tmp_buf;

    if (copy_from_user(dev->line_buffer + dev->line_buffer_size, buf, count)) {
        retval = -EFAULT;
        goto out;
    }

    dev->line_buffer_size = tmp_size;

    retval = count;
    *f_pos += count;

    pos = memchr(tmp_buf, '\n', tmp_size);
    while(pos) {
        lines_insert(dev->lines, tmp_buf, pos - tmp_buf + 1);
        tmp_size -= pos - tmp_buf + 1;
        tmp_buf = pos + 1;
        pos = memchr(tmp_buf, '\n', tmp_size);
    }
    PDEBUG("after while(pos) tmp_size = %zu", tmp_size);
    if (tmp_size) {
        memmove(dev->line_buffer, tmp_buf, tmp_size);
        dev->line_buffer_size = tmp_size;
    } else {
        /* TODO: use ksize() */
        kfree(dev->line_buffer);
        dev->line_buffer = NULL;
        dev->line_buffer_size = 0;
    }
#endif

    retval = -EFAULT; /* FIX: */
 /* out: */
    PDEBUG("retval = %d", (int)retval);
    mutex_unlock(&dev->lock);
    return retval;
}

struct file_operations dht11_fops = {
    .owner =    THIS_MODULE,
    /* .llseek =   aesd_llseek, */
    .read =     dht11_driver_read,
    .write =    dht11_driver_write,
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
    result = alloc_chrdev_region(&dev, dht11_driver_minor, 1, "dht11");
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
    return result;

 fail:
    unregister_chrdev_region(dev, 1);
    return result;
}

void dht11_driver_cleanup_module(void)
{
    dev_t devno = MKDEV(dht11_driver_major, dht11_driver_minor);

    cdev_del(&dht11_device.cdev);
    unregister_chrdev_region(devno, 1);
}

module_init(dht11_driver_init_module);
module_exit(dht11_driver_cleanup_module);
