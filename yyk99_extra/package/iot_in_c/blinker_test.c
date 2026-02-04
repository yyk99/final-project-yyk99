#include <bcm2835.h>  
#include <stdio.h>  

#include "blinker.h"

int main(int argc, char** argv)
{
    int the_pin = RPI_BPLUS_GPIO_J8_10;
    struct blinker_t bp;

    if(!blinker_init(&bp, the_pin))
        return 1;

    while (1) {
	blinker_on(&bp);
	bcm2835_delay(500);
	blinker_off(&bp);
	bcm2835_delay(500);
    }
    // bcm2835_close();
    return 0;
}

// Fairhead, Harry. Raspberry Pi IoT In C (p. 38). I/O Press. Kindle Edition. 
