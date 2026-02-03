//
//
//
//

#define  _GNU_SOURCE  
#include <stdio.h>  
#include <stdlib.h>  
#include <bcm2835.h>  
#include <time.h>  

#define BILLION 1000000000L  

int main(int argc, char** argv) 
{  
	struct timespec btime, etime;  
	volatile int i;  
	clock_gettime(CLOCK_REALTIME, &btime);  
	for (i = 0; i < 10000000; i++) {  }; 
	clock_gettime(CLOCK_REALTIME, &etime);  
	double nseconds =  (double) ((etime.tv_sec - btime.tv_sec)* BILLION)+  (double) (etime.tv_nsec - btime.tv_nsec);  
	int n = (int) 10 /nseconds * BILLION + 0.5;  
	printf("time = %f (s) \n \r",nseconds/ BILLION);  
	printf("n= %d \n\r", n);  

	return (EXIT_SUCCESS);  
}

// Fairhead, Harry. Raspberry Pi IoT In C (p. 57). I/O Press. Kindle Edition. 
