/*
Press Or Hold  
You can carry on elaborating on how to respond to a button. 
For example,  most users have grown accustomed to the idea that holding a button down for  
a longer time than a press makes something different happen. To distinguish  between a press and a hold 
all you need to do is time the difference between  line down and line up:

Fairhead, Harry. Raspberry Pi IoT In C (p. 83). I/O Press. Kindle Edition.
*/


#include <stdio.h>  
#include <stdlib.h>  
#include <bcm2835.h>  

int main(int argc, char** argv) 
{  
	if (!bcm2835_init()) 
		return 1;  
	bcm2835_gpio_fsel(RPI_BPLUS_GPIO_J8_07, BCM2835_GPIO_FSEL_INPT);  
	bcm2835_gpio_set_pud(RPI_BPLUS_GPIO_J8_07, BCM2835_GPIO_PUD_UP);  
	
	uint64_t t;  
	while (1) {  
		while (1 == bcm2835_gpio_lev(RPI_BPLUS_GPIO_J8_07));  
		t = bcm2835_st_read();  
		bcm2835_delayMicroseconds(1000);  
		while (0 == bcm2835_gpio_lev(RPI_BPLUS_GPIO_J8_07));  
		t = bcm2835_st_read() - t;  
		bcm2835_delayMicroseconds(1000);  
		if (t > 5000000) {  
			printf("Putton held \n\r");  
		} else {  
			printf("Button Push \n\r");  
		}  
		fflush(stdout);  
	}  
	return 0;  
}

/* Fairhead, Harry. Raspberry Pi IoT In C (p. 83). I/O Press. Kindle Edition. */
