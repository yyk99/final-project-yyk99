# DHT11 driver

Source code for the DHT11 char driver used with the Final Assignment

## Build the host module (DEBUG)

	make -C /usr/src/linux-headers-$(uname -r) M=`pwd` modules CC=x86_64-linux-gnu-gcc-12

or

	make

To clean:

	make clean

## Check Driver Version

	modinfo dht11_driver.ko | grep version

Or, if the module is already loaded:

	modinfo dht11_driver | grep version

## Module Parameters

- `gpio_pin` - GPIO pin number for the DHT11 sensor (default: 4, which is RPi header pin J8-07)

To load the module with a custom GPIO pin:

	insmod dht11_driver.ko gpio_pin=17

Or when using the `driver_load` script:

	./driver_load gpio_pin=17

To verify the current parameter value after loading:

	cat /sys/module/dht11_driver/parameters/gpio_pin
