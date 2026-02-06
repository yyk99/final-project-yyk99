/*
 */

#include <linux/types.h>
#include <linux/printk.h>
#include "dht11.h"

#define BCM2835_NO_DELAY_COMPATIBILITY
#include "bcm2835.h"

#define DHT11_BIT_THRESHOLD_US 50  // DHT11: <50us = '0', >50us = '1'

/*
 EXAMPLE:

  # page195_dht11 -v
  tbuf...
  88 24 24 24 71 92 25 70 25 24 31 24 24 24 25 24 25 24 24 25 70 24 24 71 71 25 24 24 24 71 24 24 25 25 25 70 71 24 70 25 71
  bytes: 26 0 19 8 53
  Checksum 53 53
  Humidity= 26.00
  Temperature= 19.80C
  Temperature= 67.64F

 */

static uint8_t dht11_get_byte(int b, int buf[])
{
    int i;
    uint8_t result = 0;

    b = (b - 1) * 8 + 1;
    for (i = b; i <= b + 7; i++) {
        result = result << 1;
        result = result | buf[i];
    }

    return result;
}

/*
  return 0 if success
*/
int dht11_get_data(struct dht11_t *self, uint8_t pin)
{
    int i;
    uint64_t t;
    int j;

    bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_write(pin, LOW);
    bcm2835_delay(18);
    bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_INPT);

    for (i = 1; i < 2000; i++) {
        if (bcm2835_gpio_lev(pin) == 0)
            break;
    };

    bcm2835_delayMicroseconds(1);
    for (j = 0; j < 41; j++) {
        for (i = 1; i < 2000; i++) {
            if (bcm2835_gpio_lev(pin) == 1)
                break;
        };
        t = bcm2835_st_read();
        for (i = 1; i < 2000; i++) {
            if (bcm2835_gpio_lev(pin) == 0)
                break;
        }

        self->tbuf[j] = (bcm2835_st_read() - t);
        self->buf[j] = self->tbuf[j] > DHT11_BIT_THRESHOLD_US;
    }

    /* printk(KERN_DEBUG "tbuf: "); */
    /* for (i = 0 ; i < 41 ; ++i) */
    /*     printk(KERN_DEBUG " %d", self->tbuf[i]); */
    /* printk(KERN_DEBUG "\n"); */

    self->byte1 = dht11_get_byte(1, self->buf);
    self->byte2 = dht11_get_byte(2, self->buf);
    self->byte3 = dht11_get_byte(3, self->buf);
    self->byte4 = dht11_get_byte(4, self->buf);
    self->byte5 = dht11_get_byte(5, self->buf);

    printk_ratelimited(KERN_DEBUG "bytes: %d %d %d %d %d\n", self->byte1, self->byte2, self->byte3, self->byte4, self->byte5);
    printk_ratelimited(KERN_DEBUG "Checksum %d %d\n", self->byte5,
           (self->byte1 + self->byte2 + self->byte3 + self->byte4) & 0xFF);

    if (self->byte5 == ((self->byte1 + self->byte2 + self->byte3 + self->byte4) & 0xFF)) {
        int neg;

        self->humidity = (int)(self->byte1 * 100 + self->byte2 * 10);
        neg = self->byte3 & 0x80;
        self->byte3 = self->byte3 & 0x7F;
        self->temperature = (int)(self->byte3 * 100 + self->byte4 * 10);
        if (neg > 0)
            self->temperature = - self->temperature;
        return 0;
    }

    /* Data inconsistent */
    return -1;
}
