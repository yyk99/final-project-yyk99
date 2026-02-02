/*
 * dht11.h
 *
 */

#ifndef DHT11_CHAR_DRIVER_H
#define DHT11_CHAR_DRIVER_H

#define DHT11_DEBUG 1

#undef PDEBUG
#ifdef DHT11_DEBUG
#  ifdef __KERNEL__
#    define PDEBUG(fmt, args...) printk( KERN_DEBUG "dht11_driver: " fmt, ## args)
#  else
#    define PDEBUG(fmt, args...) fprintf(stderr, fmt, ## args)
#  endif
#else
#  define PDEBUG(fmt, args...)
#endif

struct dht11_dev
{
	struct mutex lock;     /* mutual exclusion semaphore     */
    struct cdev cdev;     /* Char device structure      */
};

/* fix [-Wmissing-prototypes] */
int dht11_driver_open(struct inode *inode, struct file *filp);
int dht11_driver_release(struct inode *inode, struct file *filp);
ssize_t dht11_driver_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos);
ssize_t dht11_driver_write(struct file *filp, const char __user *buf, size_t count, loff_t *f_pos);
long dht11_driver_ioctl(struct file *filp, unsigned int cmd, unsigned long arg);
/* loff_t aesd_llseek(struct file *filp, loff_t off, int whence); */
int dht11_driver_init_module(void);
void dht11_driver_cleanup_module(void);

#endif
