#
#
#

CC ?= $(CROSS_COMPILE)gcc
CFLAGS ?= -g -Wall -Werror
ALL = udpserver udp_echo_client

all: $(ALL)
	@echo Done

udpserver: server.c server.h main.c Makefile
	$(CC) -o udpserver server.c main.c

udp_echo_client: udp_echo_client.c server.h Makefile
	$(CC) -o udp_echo_client udp_echo_client.c

clean:
	$(RM) *.o $(ALL)

distclean: clean
	$(RM) *~

