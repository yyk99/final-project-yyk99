#include <bcm2835.h>
#include <stdio.h>
#include <sched.h>
#include <sys/mman.h>

uint8_t getByte(int b, int buf[]);
void GetDHT22data(uint8_t pin);

int main(int argc, char** argv)
{
    const struct sched_param priority = {1};
    sched_setscheduler(0, SCHED_FIFO, &priority);
    mlockall(MCL_CURRENT | MCL_FUTURE);
    
    if (!bcm2835_init())
        return 1;
    
    bcm2835_gpio_fsel(RPI_BPLUS_GPIO_J8_07, BCM2835_GPIO_FSEL_INPT);
    bcm2835_delayMicroseconds(1000);
    GetDHT22data(RPI_BPLUS_GPIO_J8_07);

    return 0;
}

void GetDHT22data(uint8_t pin)
{
    bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_write(pin, LOW);
    bcm2835_delayMicroseconds(18000); // delay(18)
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

    printf("tbuf...\n");
    for (i = 0 ; i < 41 ; ++i)
	printf(" %d", tbuf[i]);
    printf("\n");

    int byte1 = getByte(1, buf);
    int byte2 = getByte(2, buf);
    int byte3 = getByte(3, buf);
    int byte4 = getByte(4, buf);
    int byte5 = getByte(5, buf);


    printf("bytes: %d %d %d %d %d\n", byte1, byte2, byte3, byte4, byte5); 

    printf("Checksum %d %d\n", byte5, (byte1 + byte2 + byte3 + byte4) & 0xFF);

    float humidity = (float)(byte1 << 8 | byte2) / 10.0;
    printf("Humidity= %f\n", humidity);

    float temperature;
    int neg = byte3 & 0x80;
    byte3 = byte3 & 0x7F;
    temperature = (float)(byte3 << 8 | byte4) / 10.0;
    if (neg > 0)
        temperature = -temperature;
    printf("Temperature= %f\n", temperature);
    return;
}  

uint8_t getByte(int b, int buf[])
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

// Fairhead, Harry.Raspberry Pi IoT In C(p. 196).I / O Press.Kindle Edition.
