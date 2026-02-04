#
#
#

CC ?= $(CROSS_COMPILE)gcc
CFLAGS ?= -g -Wall -Werror
ALL = udpserver udpclient

all: $(ALL)
	@echo Done

udpserver: server.c server.h main.c Makefile
	$(CC) -o udpserver server.c main.c

udpclient: udpclient.c server.h Makefile
	$(CC) -o udpclient udpclient.c

clean:
	$(RM) *.o $(ALL)

distclean: clean
	$(RM) *~

