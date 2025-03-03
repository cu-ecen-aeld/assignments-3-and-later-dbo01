CC      = gcc
CFLAGS  = -g
RM      = rm -f

default: server
all: server

server: aesdsocket.c
	$(CROSS_COMPILE)$(CC) $(CFLAGS) -o aesdsocket aesdsocket.c

clean veryclean:
	$(RM) server