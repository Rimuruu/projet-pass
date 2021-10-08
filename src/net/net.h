#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> 
#include <arpa/inet.h>
#include <string.h>
#include <stdbool.h>
#include "errhandlers.h"
#include "game.h"
#include <errno.h>

#pragma once


bool make_sockaddr(struct sockaddr_in* serv_addr,
                    char * ip_addr,
                    int port
                    );



bool init_socket(int* s);
bool init_bind(int* s,struct sockaddr * my_addr);
bool sendGame(int* s,struct Game* game,struct sockaddr * dest_addr);
bool recvGame(int* s,struct Game* game,struct sockaddr * src_addr);
