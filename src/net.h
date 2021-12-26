
#pragma once
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

struct Packet
{
    uint8_t data[MAX];
    size_t size;
};

bool make_sockaddr(struct sockaddr_in *serv_addr,
                   char *ip_addr, uint16_t port);

bool init_socket(int *s);

bool init_bind(
    int *s, struct sockaddr *my_addr);

bool init_listen(int *s);

bool accept_client(int *client_socket,
                   int *s, struct sockaddr *client_addr, socklen_t *addrlen);

bool connection_server(
    int *s, struct sockaddr *server_addr, socklen_t addrlen);

bool close_socket(int *s);

bool send_packet(struct Packet *p,
                 int socket, FILE *f_w);
bool recv_packet(struct Packet *p,
                 int socket, FILE *f_r);
bool recv_unknown_packet(struct Packet *p,
                         int socket, FILE *f_r);

bool set_packet(struct Packet *p,
                uint8_t *data, size_t size_struct, enum typeV type);

bool recv_from(struct Packet *p,
               struct Client_info *clients, int c, int *listen_s);

bool send_to(
    struct Packet *p, struct Client_info *clients, int c, int *listen_s);

bool send_disconnect(struct Client_info client);

bool handle_full(int *socket);