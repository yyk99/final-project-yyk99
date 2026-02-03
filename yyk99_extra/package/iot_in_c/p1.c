#include <bcm2835.h>  
#include <stdio.h>  

int main(int argc, char** argv)
{
    int the_pin = RPI_BPLUS_GPIO_J8_10;

    if (!bcm2835_init())
	return 1;
    bcm2835_gpio_fsel(the_pin, BCM2835_GPIO_FSEL_OUTP);
    while (1) {
	bcm2835_gpio_write(the_pin, HIGH);
	bcm2835_delay(500);
	bcm2835_gpio_write(the_pin, LOW);
	bcm2835_delay(500);
    }
    bcm2835_close();
    return 0;
}

// Fairhead, Harry. Raspberry Pi IoT In C (p. 38). I/O Press. Kindle Edition. 
