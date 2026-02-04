/*
 */

#include <linux/types.h>
#include "dht11.h"
/* #include "bcm2835.h" */

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
#if 0
    b = (b - 1) * 8 + 1;
    for (i = b; i <= b + 7; i++) {
        result = result << 1;
        result = result | buf[i];
    }
#else
    (void)i;
    /* TODO: implement properly */
    /* bytes: 26 0 19 8 53 */
    switch(b){
    case 1: result = 26; break;
    case 2: result = 0; break;
    case 3: result = 19; break;
    case 4: result = 8; break;
    case 5: result = 53; break;
    default:
        break;
    }
#endif
    return result;
}


/*
  return 0 if success
*/
int dht11_get_data(struct dht11_t *self, uint8_t pin)
{
#if 0
    bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_write(pin, LOW);
    delay(18);
    bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_INPT);
    int i;
    for (i = 1; i < 2000; i++) {
        if (bcm2835_gpio_lev(pin) == 0)
	    break;
    };

    uint64_t t;
    int buf[41];
    int tbuf[41];
    int j;

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

        tbuf[j] = (bcm2835_st_read() - t);
        buf[j] = tbuf[j] > 50;
    }

    if(verbose) {
        printf("tbuf...\n");
        for (i = 0 ; i < 41 ; ++i)
            printf(" %d", tbuf[i]);
        printf("\n");
    }

    int byte1 = getByte(1, buf);
    int byte2 = getByte(2, buf);
    int byte3 = getByte(3, buf);
    int byte4 = getByte(4, buf);
    int byte5 = getByte(5, buf);

    if(verbose) {
        printf("bytes: %d %d %d %d %d\n", byte1, byte2, byte3, byte4, byte5);
        printf("Checksum %d %d\n", byte5, (byte1 + byte2 + byte3 + byte4) & 0xFF);
    }

    if (byte5 ==  ((byte1 + byte2 + byte3 + byte4) & 0xFF)) {
        float humidity = (float)byte1 + (float) byte2 / 10.0;
        printf("Humidity= %.2f\n", humidity);

        float temperature;
        int neg = byte3 & 0x80;
        byte3 = byte3 & 0x7F;
        temperature = (float)byte3  + (float)byte4 / 10.0;
        if (neg > 0)
            temperature = -temperature;
        printf("Temperature= %.2fC\n", temperature);
        printf("Temperature= %.2fF\n", convertCtoF(temperature));
    } else {
        printf("Data inconsistent\n");
    }
    return;
#else
    self->byte1 = dht11_get_byte(1, self->buf);
    self->byte2 = dht11_get_byte(2, self->buf);
    self->byte3 = dht11_get_byte(3, self->buf);
    self->byte4 = dht11_get_byte(4, self->buf);
    self->byte5 = dht11_get_byte(5, self->buf);

    if (self->byte5 ==  ((self->byte1 + self->byte2 + self->byte3 + self->byte4) & 0xFF)) {
        self->humidity = (int)(((float)self->byte1 + (float) self->byte2 / 10.0) * 100);

        int neg = self->byte3 & 0x80;
        self->byte3 = self->byte3 & 0x7F;
        self->temperature = (int)(((float)self->byte3  + (float)self->byte4 / 10.0) * 100);
        if (neg > 0)
            self->temperature = - self->temperature;
        return 0;
    }
    /* Data inconsistent */
    return -1;
#endif
}
