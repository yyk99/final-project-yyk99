/*
 * dht11_ioctl.h
 */

#ifndef DHT11_IOCTL_H
#define DHT11_IOCTL_H

#ifdef __KERNEL__
#include <asm-generic/ioctl.h>
#include <linux/types.h>
#else
#include <sys/ioctl.h>
#include <stdint.h>
#endif

/**
 * A structure to be passed by IOCTL from user space to kernel space
 */
struct dht11_seekto {
    uint32_t cmd;
    uint32_t cmd_aux;
};

/* Pick an arbitrary unused value from */
/* https://github.com/torvalds/linux/blob/master/Documentation/userspace-api/ioctl/ioctl-number.rst */
#define DHT11_IOC_MAGIC 0x16

/* Define a write command from the user point of view, use command number 1 */
#define DHT11CHAR_IOCSEEKTO _IOWR(DHT11_IOC_MAGIC, 1, struct dht11_seekto)
/**
 * The maximum number of commands supported, used for bounds checking
 */
#define DHT11CHAR_IOC_MAXNR 1

#endif
