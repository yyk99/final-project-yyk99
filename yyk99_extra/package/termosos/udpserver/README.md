# UDP Server for DHT11 Sensor

A UDP server that reads temperature and humidity data from the DHT11 sensor
via the `/dev/dht11` character device driver.

## Usage

    udpserver [-d] [-p port]

Options:
- `-d` -- run as daemon
- `-p port` -- specify port number (default: 9001)
- `-h` -- show usage

## Client Protocol

The server listens on UDP port 9001. Clients send single-character commands
and receive text responses.

### Commands

| Command | Description                        |
|---------|------------------------------------|
| `v`     | Show server version                |
| `?`     | Show available commands            |
| `t`     | Read temperature from DHT11 sensor |
| `h`     | Read humidity from DHT11 sensor    |

Any unrecognized command returns a help summary.

### Response Format

Successful responses are prefixed with `OK `:

    OK udpserver version 1.0
    OK T: 23.50C
    OK H: 45.00%

Error responses are prefixed with `ERROR `:

    ERROR Cannot read DHT11 sensor

## UDP Client

A simple interactive client is provided for testing:

    udpclient [server-ip]

Default server address is `127.0.0.1`. Type commands at the `>` prompt.

## Build

    make
