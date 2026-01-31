#
#
#

CC ?= $(CROSS_COMPILE)gcc
CFLAGS ?= -g -Wall -Werror

all: udpserver
	@echo Done

udpserver: server.c server.h main.c
	$(CC) -o udpserver server.c main.c

clean:
	$(RM) *.o udpserver

distclean: clean
	$(RM) *~

