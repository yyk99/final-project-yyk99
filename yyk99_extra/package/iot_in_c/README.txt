If you do want to explore the GPIO from the command line, 
you need to  install some tools that have been created mainly as examples of 
using the new  device interface. To do this you need to first install them and the gpiod library  that you will use later:  

sudo apt-get install gpiod libgpiod-dev libgpiod-doc

// Fairhead, Harry. Raspberry Pi IoT In C (p. 99). I/O Press. Kindle Edition. 

## How to build

	cmake -B build -S .
	cmake --build build

in case if the library is not available:

	# Download the latest version
	cd /tmp
	wget http://www.airspayce.com/mikem/bcm2835/bcm2835-1.73.tar.gz

	# Extract
	tar zxvf bcm2835-1.73.tar.gz
	cd bcm2835-1.73

	# Configure, compile, and install
	./configure
	make
	sudo make check
	sudo make install

## How to run

	sudo ./build/page195_dht11
