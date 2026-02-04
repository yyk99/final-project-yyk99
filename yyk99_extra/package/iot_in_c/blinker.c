/*
*
*/

#include "blinker.h"

#include <bcm2835.h>  
#include <stdio.h>  
#include <stdbool.h>
#include <string.h>

bool blinker_init(struct blinker_t *self, int pin)
{
    static bool initialized = false;

    memset(self, 0, sizeof(*self));
    self->pin = -1;

    if (!initialized) {
        initialized = bcm2835_init();
    }
    if (initialized) {
        bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_OUTP);
        self->pin = pin;
    }
    return initialized; 
}

void blinker_on(struct blinker_t *self)
{
    bcm2835_gpio_write(self->pin, HIGH); 
}

void blinker_off(struct blinker_t *self)
{
    bcm2835_gpio_write(self->pin, LOW);
}

// Local Variables:
// mode: c
// c-basic-offset: 4
// tab-width: 4
// indent-tabs-mode: nil
// End:
