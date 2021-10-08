CC = gcc
SRC = src
CFLAG = -Wall
INC  = -I "src/err" -I "src/data" -I "src/net"
BIN = bin

all : bin client server

bin :
	mkdir bin/

game.o : $(SRC)/data/game.c $(SRC)/err/errhandlers.h
	$(CC) -o  $(BIN)/game.o -c $(SRC)/data/game.c $(CFLAG) $(INC)


net.o : $(SRC)/net/net.c $(SRC)/err/errhandlers.h $(SRC)/data/game.h
	$(CC) -o  $(BIN)/net.o -c $(SRC)/net/net.c $(CFLAG) $(INC)

stdmsghandlers.o : $(SRC)/err/stdmsghandlers.c
	$(CC) -o  $(BIN)/stdmsghandlers.o -c $(SRC)/err/stdmsghandlers.c $(CFLAG) $(INC)

client.o : $(SRC)/client.c $(SRC)/err/errhandlers.h $(SRC)/data/game.h $(SRC)/net/net.h
	$(CC) -o  $(BIN)/client.o -c $(SRC)/client.c $(CFLAG) $(INC)

server.o : $(SRC)/server.c $(SRC)/err/errhandlers.h $(SRC)/data/game.h $(SRC)/net/net.h
	$(CC) -o  $(BIN)/server.o -c $(SRC)/server.c $(CFLAG) $(INC)

client : client.o stdmsghandlers.o game.o net.o
	$(CC) -o client $(BIN)/client.o $(BIN)/stdmsghandlers.o $(BIN)/game.o $(BIN)/net.o $(CFLAG) 


server : server.o stdmsghandlers.o game.o net.o
	$(CC) -o server $(BIN)/server.o $(BIN)/stdmsghandlers.o $(BIN)/game.o $(BIN)/net.o $(CFLAG) 


clean :
	rm -rf bin/
	rm client
	rm server