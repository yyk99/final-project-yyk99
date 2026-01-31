#
#
#

CC ?= $(CROSS_COMPILE)gcc
CFLAGS ?= -g -Wall -Werror

all: udpserver udp_echo_client
	@echo Done

udpserver: server.c server.h main.c
	$(CC) -o udpserver server.c main.c

udp_echo_client: udp_echo_client.c server.h
	$(CC) -o udp_echo_client udp_echo_client.c

clean:
	$(RM) *.o udpserver

distclean: clean
	$(RM) *~

