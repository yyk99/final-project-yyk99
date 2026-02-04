#ifndef TERMOSOS_DHT11_H
#define TERMOSOS_DHT11_H

/* bcm2835.h

   C and C++ support for Broadcom BCM 2835 as used in Raspberry Pi

   Author: Mike McCauley
   Copyright (C) 2011-2013 Mike McCauley
   $Id: bcm2835.h,v 1.26 2020/01/11 05:07:13 mikem Exp mikem $
*/

struct dht11_t {
    int humidity; /* hum * 100 */
    int temperature; /* tem in C * 100 */
    int buf[41];
    int tbuf[41];

    int byte1;
    int byte2;
    int byte3;
    int byte4;
    int byte5;
};

extern int dht11_get_data(struct dht11_t *self, uint8_t pin);

#endif /* TERMOSOS_DHT11_H */
