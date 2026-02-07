# DHT11 driver

A Linux kernel character device driver for the DHT11 temperature and humidity
sensor on Raspberry Pi. The driver communicates with the sensor using
bit-banged GPIO via the BCM2835 peripheral library adapted for kernel space.

## Purpose

The driver exposes the DHT11 sensor as a character device at `/dev/dht11`.
It is intended for use with the Raspberry Pi as part of the Final Assignment.

## Driver Behavior

- When a process opens `/dev/dht11`, the driver triggers the DHT11 sensor
  protocol on the configured GPIO pin and reads 40 bits of data (humidity,
  temperature, and checksum).
- If the checksum is valid, the driver stores a formatted text string such as
  `T: 23.50C H: 45.00%` in an internal buffer.
- If the checksum fails, the buffer contains `Inconsistent data`.
- Subsequent `read()` calls return the buffered text. The data does not change
  until the device is closed and reopened.
- Only one process may open the device at a time. Additional attempts return
  `-EBUSY`.
- Closing the device releases the lock so another process can read the sensor.

## Example Usage

	cat /dev/dht11
	T: 23.50C H: 45.00%

Multiple reads in a script:

	value=$(cat /dev/dht11)
	echo "$value"

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
