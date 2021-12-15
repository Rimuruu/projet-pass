
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
#include <string.h>
#include <sys/select.h>
#include "enumvalue.h"

#pragma once
#define MAX 1024
struct Client_info
{
    struct sockaddr_in addr;
    int socket;
    socklen_t s_len;
    FILE *f_w;
    FILE *f_r;
    bool status;
};

struct Server_info
{
    FILE *f_w;
    FILE *f_r;
    int socket;
};

bool make_sockaddr(struct sockaddr_in *serv_addr,
                   char *ip_addr,
                   uint16_t port);

bool init_socket(int *s);

bool init_bind(
    int *s, struct sockaddr *my_addr);

bool init_listen(int *s);

bool accept_client(int *client_socket,
                   int *s, struct sockaddr *client_addr, socklen_t *addrlen);

bool connection_server(
    int *s, struct sockaddr *server_addr, socklen_t addrlen);

bool close_socket(int *s);

bool send_packet(uint8_t *packet,
                 int socket, FILE *f_w);
bool recv_packet(uint8_t *packet,
                 int socket, FILE *f_r);

bool set_packet(uint8_t *packet,
                uint8_t *data, size_t size_struct, enum typeV typ);

bool recv_from(uint8_t* packet,
                struct Client_info* clients,int c);
