
CC=	gcc
CFLAGS= -Wall -Werror 


all :client	server

debug : CFLAGS += -DDEBUG
debug :client	server 

game.o: errhandlers.h

net.o: errhandlers.h game.h enumvalue.h

stdmsghandlers.o:	errhandlers.h

client.o:		errhandlers.h	game.h	net.h enumvalue.h

server.o:		errhandlers.h	game.h	net.h enumvalue.h

client:		stdmsghandlers.o	game.o	net.o

server:		stdmsghandlers.o	game.o	net.o 

clean:
	rm -f *.o
	rm -f client
	rm -f server

.PHONY:	all
