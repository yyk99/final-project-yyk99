# DHT11 driver

Source code for the DHT11 char driver used with the Final Assignment

## Build the host module (DEBUG)

	make -C /usr/src/linux-headers-$(uname -r) M=`pwd` modules CC=x86_64-linux-gnu-gcc-12

or

	make

To clean:

	make clean
