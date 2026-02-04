#include <bcm2835.h>
#include <stdio.h>
#include <sched.h>
#include <sys/mman.h>

#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <assert.h>

/* According to POSIX.1-2001, POSIX.1-2008 */
#include <sys/select.h>

/* According to earlier standards */
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <bcm2835.h>
#include "temperature_utils.h"
#include "blinker.h"

bool verbose = false;

struct rec_t {
    uint32_t stamp;
    int32_t x, y, z;
    uint32_t h, c, f;
};

uint8_t getByte(int b, int buf[]);

void GetDHT22data(uint8_t pin, struct rec_t *tbuf);

void read_temperature(struct rec_t *tbuf);

struct blinker_t the_pin;

int loop() 
{
    const int destPort = 2390;

    const int PIN = RPI_BPLUS_GPIO_J8_10; // pin #10
    
    struct sockaddr_in dest;
    struct sockaddr_in local;
    struct rec_t rec;

    blinker_init(&the_pin, PIN);

    local.sin_family = AF_INET;
    local.sin_port = htons(destPort);
    local.sin_addr.s_addr = INADDR_ANY;

    dest.sin_family = AF_INET;
    dest.sin_port = htons(0);
    dest.sin_addr.s_addr = INADDR_ANY;

    int s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    assert(s > 0);
    bind(s, (struct sockaddr*)&local, sizeof(local));

    while(1) {
        char buffer[1024];
        int dest_size = sizeof(dest);

        fd_set fdset;
        FD_ZERO(&fdset);
        FD_SET(s, &fdset);
        struct timeval tmo = { 5, 0 }; // one second

        int ok = select(s + 1, &fdset, NULL, NULL, &tmo);
        if(ok < 0) {
            perror("select error");
            exit(1);
        }
        if (ok > 0) {
            ok = recvfrom(s, buffer, sizeof(buffer), 0,
                        (struct sockaddr *)&dest, &dest_size);

            printf("ok == %d\n", ok);
            if (strncmp( buffer, "CONNECT", 7 ) == 0) {
                blinker_on(&the_pin);
                bcm2835_delay(100);
                
                const char *reply = "OK Connected\r\n";
                ok = sendto(s, reply, strlen(reply), 0, (struct sockaddr*)&dest, dest_size);
                assert(ok == strlen(reply));
            } else if (strncmp( buffer, "CLOSE", 5 ) == 0) {
                blinker_off(&the_pin);
                bcm2835_delay(100);
                
                const char *reply = "OK Closed\r\n";
                ok = sendto(s, reply, strlen(reply), 0, (struct sockaddr*)&dest, dest_size);
                assert(ok == strlen(reply));
                memset(&dest, 0, sizeof(dest));
            }
        }
        if (dest.sin_port) {
            read_temperature(&rec);

            ok = sendto(s, &rec, sizeof(rec), 0, (struct sockaddr*)&dest, dest_size);
            assert(ok == sizeof(rec));
        }
    }

    return 0;
}

int main(int argc, char** argv)
{
    if(argc > 1 && argv[1][0] == '-' && argv[1][1] == 'v')
        verbose = true;

    const struct sched_param priority = {1};
    sched_setscheduler(0, SCHED_FIFO, &priority);
    mlockall(MCL_CURRENT | MCL_FUTURE);
    
    if (!bcm2835_init())
        return 1;
 
    loop(); // DEBUG

    return 0;
}

void GetDHT22data(uint8_t pin, struct rec_t *record)
{
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

        time_t t1;
        time(&t1);

        record->stamp = t1;

        record->x = -1;
        record->y = -1;
        record->z = -1;
        record->c = temperature * 100;
        record->f = convertCtoF(temperature) * 100;
        record->h = humidity * 100;
    } else {
        printf("Data inconsistent\n");
    }
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


void read_temperature(struct rec_t *tbuf)
{
    bcm2835_gpio_fsel(RPI_BPLUS_GPIO_J8_07, BCM2835_GPIO_FSEL_INPT);
    bcm2835_delayMicroseconds(1000);
    GetDHT22data(RPI_BPLUS_GPIO_J8_07, tbuf);
}

// Local Variables:
// mode: c
// c-basic-offset: 4
// tab-width: 4
// indent-tabs-mode: nil
// End:
