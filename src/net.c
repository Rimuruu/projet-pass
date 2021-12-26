#include "net.h"

void packet_size(size_t *size,
                 uint8_t type)
{
    switch (type)
    {
    case MSG:
        (*size) = sizeof(struct Message);
        break;
    case MAXWORD:
        (*size) = sizeof(uint8_t);
        break;
    case GLIST:
        (*size) = sizeof(struct WordList);
        break;
    case HLIST:
        (*size) = sizeof(struct WordList);
        break;
    case WGUESS:
        (*size) = sizeof(struct Word);
        break;
    case WHINT:
        (*size) = sizeof(struct Word);
        break;
    case WORD:
        (*size) = sizeof(struct Word);
        break;
    default:
        (*size) = 0;
        break;
    }
}

bool make_sockaddr(struct sockaddr_in *serv_addr,
                   char *ip_addr, uint16_t port)
{
    if (inet_aton(ip_addr, &(serv_addr->sin_addr)) == 0)
    {

        return true;
    }

    serv_addr->sin_family = AF_INET;
    serv_addr->sin_port = htons(port);

    return false;
}

bool init_socket(int *s)
{
    (*s) = socket(AF_INET, SOCK_STREAM, 0);
    if ((*s) == -1)
    {
        errmsgf("err socket init\n");
        return true;
    }

    return false;
}

bool init_bind(int *s,
               struct sockaddr *my_addr)
{
    debug_print("socket debug %d %p\n", *s, s);
    if (bind(*s, my_addr, sizeof(struct sockaddr)) == -1)
    {
        errmsgf("err bind %s\n", strerror(errno));
        return true;
    }

    return false;
}

bool init_listen(int *s)
{
    if (listen(*s, 2) == -1)
    {
        errmsgf("err listen\n");
        return true;
    }

    return false;
}

bool accept_client(int *client_socket,
                   int *s, struct sockaddr *client_addr, socklen_t *addrlen)
{
    int socket = accept(*s, client_addr, addrlen);
    if (socket == -1)
    {
        errmsgf("err socket client\n");
        return true;
    }
    *client_socket = socket;
    return false;
}

bool connection_server(
    int *s, struct sockaddr *server_addr, socklen_t addrlen)
{
    if (connect(*s, server_addr, addrlen) == -1)
    {
        switch (errno)
        {
        case ECONNREFUSED:
            errmsgf("Server offline\n");
            break;
        case ENETUNREACH:
            errmsgf("Network is unreachable\n");
            break;
        case ETIMEDOUT:
            errmsgf("Server full\n");
            break;
        }

        return true;
    }

    return false;
}

bool close_socket(int *s)
{
    if (close(*s) == -1)
    {
        errmsgf("err close socket\n");
        return true;
    }

    return false;
}

bool set_packet(struct Packet *p,
                uint8_t *data, size_t size_struct, enum typeV type)
{
    if (size_struct > 1023)
    {
        errmsgf("err packet too big \n");
        return true;
    }
    p->data[0] = (uint8_t)type;
    if (size_struct > 0)
    {

        memcpy(p->data + 1, data, size_struct);
    }
    p->size = size_struct + 1;
    return false;
}

bool send_packet(struct Packet *p,
                 int socket, FILE *f_w)
{

    size_t writeV = 0;

    writeV = fwrite(p->data, sizeof(uint8_t), 1, f_w);

    debug_print("%d %d bytes data write\n", p->data[0], (int)writeV);
    if (writeV != 1)
    {
        debug_print("%s\n", strerror(errno));
        return true;
    }
    if (ferror(f_w) != 0)
    {
        debug_print("ferror %s\n", strerror(errno));
        return true;
    }
    if (fflush(f_w) == EOF)
    {
        return true;
    }
    if (p->size - 1 != 0)
    {
        writeV = fwrite(p->data + 1, sizeof(uint8_t), p->size - 1, f_w);

        debug_print("%d bytes data write\n", (int)writeV);
        if (writeV != p->size - 1)
        {
            debug_print("%s\n", strerror(errno));
            return true;
        }
        if (ferror(f_w) != 0)
        {
            debug_print("ferror %s\n", strerror(errno));
            return true;
        }
        if (fflush(f_w) == EOF)
        {
            return true;
        }
    }

    return false;
}
bool recv_packet(struct Packet *p,
                 int socket, FILE *f_r)
{

    size_t readV = 0;
    readV = fread(p->data, sizeof(uint8_t), p->size, f_r);
    debug_print("%d bytes data read\n", (int)readV);
    if (readV == 0)
    {
        if (feof(f_r) != 0)
        {
            debug_print("Socket disconnect \n");
            return true;
        }
        if (ferror(f_r) != 0)
        {
            errmsgf("Error read \n");
            return true;
        }
    }
    if (readV != p->size)
    {
        return true;
    }
    if (fflush(f_r) == EOF)
    {
        return true;
    }
    debug_print("Packet type: %d \n", (int)p->data[0]);

    return false;
}

bool recv_unknown_packet(struct Packet *p,
                         int socket, FILE *f_r)
{

    size_t readV = 0;

    readV = fread(p->data, sizeof(uint8_t), 1, f_r);

    debug_print("%d bytes data read\n", (int)readV);
    if (readV == 0)
    {
        if (feof(f_r) != 0)
        {
            debug_print("Socket disconnect \n");
            return true;
        }
        if (ferror(f_r) != 0)
        {
            debug_print("Error read \n");
            return true;
        }
    }
    if (fflush(f_r) == EOF)
    {
        return true;
    }

    packet_size(&(p->size), p->data[0]);
    debug_print("Waiting %d %" PRIu8 " bytes data read\n", (int)p->size, p->data[0]);
    readV = fread(p->data + 1, sizeof(uint8_t), p->size, f_r);

    if (readV != p->size)
    {
        return true;
    }
    if (fflush(f_r) == EOF)
    {
        return true;
    }

    debug_print("Packet type: %d \n", (int)p->data[0]);

    return false;
}

bool recv_from(struct Packet *p,
               struct Client_info *clients, int c)
{
    fd_set set;
    int r;

    int maxfd = clients[0].socket > clients[1].socket ? clients[0].socket : clients[1].socket;
    struct Packet tmp;
    tmp.size = p->size;
    debug_print("Waiting client %d\n", c);
    //We check every socket as we wait a packet from client c
    while (true)
    {
        struct timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
        FD_ZERO(&set);
        FD_SET(clients[0].socket, &set);
        FD_SET(clients[1].socket, &set);
        r = select(maxfd + 1, &set, NULL, NULL, &timeout);
        if (r == -1)
        {
            return false;
        }
        if (r > 0)
        {
            if (FD_ISSET(clients[0].socket, &set))
            {
                if (c == 0)
                {
                    if (recv_packet(p, clients[0].socket, clients[0].f_r))
                    {
                        if (feof(clients[0].f_r) != 0)
                        {

                            if (send_disconnect(clients[1]))
                            {
                            }
                            errmsgf("Player disconnect\n");
                        }
                        return true;
                    }
                    return false;
                }
                else
                {
                    if (recv_packet(&tmp, clients[0].socket, clients[0].f_r))
                    {

                        if (feof(clients[0].f_r) != 0)
                        {

                            if (send_disconnect(clients[1]))
                            {
                            }
                            errmsgf("Player disconnect\n");
                        }
                        return true;
                    }
                    debug_print("Didn't ask client\n");
                }
            }
            if (FD_ISSET(clients[1].socket, &set))
            {
                if (c == 1)
                {
                    if (recv_packet(p, clients[1].socket, clients[1].f_r))
                    {
                        if (feof(clients[1].f_r) != 0)
                        {

                            if (send_disconnect(clients[0]))
                            {
                            }
                            errmsgf("Player disconnect\n");
                        }
                        return true;
                    }
                    return false;
                }
                else
                {
                    if (recv_packet(&tmp, clients[1].socket, clients[1].f_r))
                    {
                        if (feof(clients[1].f_r) != 0)
                        {

                            if (send_disconnect(clients[0]))
                            {
                            }
                            errmsgf("Player disconnect\n");
                        }
                        return true;
                    }
                    debug_print("Didn't ask client\n");
                }
            }
        }
    }
}

bool send_disconnect(struct Client_info client)
{
    fd_set set;
    int r;
    struct Packet p;
    struct Packet tmp;
    struct timeval timeout;
    struct Message msg;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    if (initMsg(&msg, (uint8_t *)"Game over \n The other player has disconnected", 46))
    {
        debug_print("init\n");
        return true;
    }

    if (set_packet(&p, (uint8_t *)&msg, sizeof(struct Message), MSG))
    {
        return true;
    }

    FD_ZERO(&set);
    FD_SET(client.socket, &set);
    r = select(client.socket + 1, &set, NULL, NULL, &timeout);
    if (r == -1)
    {
        return false;
    }
    //before sending the packet we check every socket
    if (r > 0)
    {
        if (FD_ISSET(client.socket, &set))
        {

            if (recv_packet(&tmp, client.socket, client.f_r))
            {
                if (feof(client.f_r) != 0)
                {
                    errmsgf("Player disconnect\n");
                }
                return true;
            }
        }
    }
    if (send_packet(&p, client.socket, client.f_w))
    {
        return true;
    }
    return false;
}

bool send_to(
    struct Packet *p, struct Client_info *clients, int c)
{
    fd_set set;
    int r;
    struct Packet tmp;
    struct timeval timeout;
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    int maxfd = clients[0].socket > clients[1].socket ? clients[0].socket : clients[1].socket;
    FD_ZERO(&set);
    FD_SET(clients[0].socket, &set);
    FD_SET(clients[1].socket, &set);
    r = select(maxfd + 1, &set, NULL, NULL, &timeout);
    if (r == -1)
    {
        return false;
    }
    //before sending the packet we check every socket
    if (r > 0)
    {
        if (FD_ISSET(clients[0].socket, &set))
        {

            if (recv_packet(&tmp, clients[0].socket, clients[0].f_r))
            {
                if (feof(clients[0].f_r) != 0)
                {

                    if (send_disconnect(clients[1]))
                    {
                    }
                    errmsgf("Player disconnect\n");
                }
                return true;
            }
            debug_print("Didn't ask client\n");
        }
        if (FD_ISSET(clients[1].socket, &set))
        {

            if (recv_packet(&tmp, clients[1].socket, clients[1].f_r))
            {
                if (feof(clients[1].f_r) != 0)
                {

                    if (send_disconnect(clients[0]))
                    {
                    }
                    errmsgf("Player disconnect\n");
                }
                return true;
            }
            debug_print("Didn't ask client\n");
        }
    }

    if (send_packet(p, clients[c].socket, clients[c].f_w))
    {
        return true;
    }
    return false;
}
