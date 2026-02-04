/*
FSM Button  
As an example, let’s implement the simple button given earlier in the chapter.  
The first version used multiple loops to wait for changes in the state of the  input line. 
The finite state version uses only a single polling loop:  
*/

#include <stdio.h>  
#include <stdlib.h>  
#include <bcm2835.h>  

int main(int argc, char** argv) {  
	if (!bcm2835_init()) 
		return 1;
	bcm2835_gpio_fsel(RPI_BPLUS_GPIO_J8_07, BCM2835_GPIO_FSEL_INPT);
	bcm2835_gpio_set_pud(RPI_BPLUS_GPIO_J8_07, BCM2835_GPIO_PUD_UP);
	uint64_t t;
	int s = 0;
	int i;
	while (1) {  
		t = bcm2835_st_read();
		i = bcm2835_gpio_lev(RPI_BPLUS_GPIO_J8_07);
		switch (s) {
		case 0: //Button not pushed
			if (!i) {
				s = 1;
				printf("Button Push \n\r");
				fflush(stdout);
			}
			break;
		case 1: //Button pushed
			if (i) {
				s = 0;
			}
			break;
		default:
			s = 0;
		}
		t=bcm2835_st_read() - t;
		bcm2835_delayMicroseconds(1000 - t);
	}
	return 0;
}

// Fairhead, Harry. Raspberry Pi IoT In C (p. 89). I/O Press. Kindle Edition. 