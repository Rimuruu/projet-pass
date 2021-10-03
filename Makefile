CC = gcc
SRC = src
CFLAG = -Wall
INC  = -I "src/err" -I "src/data"
BIN = bin

all : bin client

bin :
	mkdir bin/

game.o : $(SRC)/data/game.c $(SRC)/err/errhandlers.h
	$(CC) -o  $(BIN)/game.o -c $(SRC)/data/game.c $(CFLAG) $(INC)

stdmsghandlers.o : $(SRC)/err/stdmsghandlers.c
	$(CC) -o  $(BIN)/stdmsghandlers.o -c $(SRC)/err/stdmsghandlers.c $(CFLAG) $(INC)

main.o : $(SRC)/main.c $(SRC)/err/errhandlers.h
	$(CC) -o  $(BIN)/main.o -c $(SRC)/main.c $(CFLAG) $(INC)

client : main.o stdmsghandlers.o game.o
	$(CC) -o client $(BIN)/main.o $(BIN)/stdmsghandlers.o $(BIN)/game.o $(CFLAG) 

clean :
	rm -rf bin/
	rm client