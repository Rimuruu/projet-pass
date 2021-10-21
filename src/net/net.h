#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h> 
#include <arpa/inet.h>
#include <string.h>
#include <stdbool.h>
#include "errhandlers.h"
#include "game.h"
#include <errno.h>
#include <unistd.h>

#pragma once


struct Client_info{
    struct sockaddr_in addr;
    int socket;
    socklen_t s_len;
};

bool make_sockaddr(struct sockaddr_in* serv_addr,
                    char * ip_addr,
                    int port
                    );



bool init_socket(int* s);

bool init_bind(
                int* s,struct sockaddr * my_addr);

bool init_listen(int* s);

bool accept_client(int* client_socket,
                int* s,struct sockaddr* client_addr,socklen_t* addrlen);

bool connection_server(
                int* s,struct sockaddr* server_addr,socklen_t addrlen);


bool sendGame(int* s,
            struct Game* game,struct sockaddr * dest_addr);

bool recvGame(int* s,
            struct Game* game,struct sockaddr * src_addr);

bool close_socket(int * s);
