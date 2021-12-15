#include "net.h"

bool make_sockaddr(struct sockaddr_in *serv_addr,
                   char *ip_addr,
                   uint16_t port)
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

    if (bind(*s, my_addr, sizeof((*my_addr))) == -1)
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

bool set_packet(uint8_t *packet,
                uint8_t *data, size_t size_struct, enum typeV type)
{
    if (size_struct > 1023)
    {
        errmsgf("err packet too big \n");
        return true;
    }
    packet[0] = type;
    if (size_struct > 0)
    {

        memcpy(packet + 1, data, size_struct);
    }
    return false;
}

bool send_packet(uint8_t *packet,
                 int socket, fd_set *set2, FILE *f_w)
{
    struct timeval tv = {2, 0};
    fd_set set;
    int r;
    while (true)
    {
        FD_ZERO(&set);
        FD_SET(socket, &set);
        r = select(socket + 1, NULL, &set, NULL, &tv);
        if (r == -1)
        {
            return true;
        }
        if (FD_ISSET(socket, &set))
        {
            size_t writeV;

            writeV = fwrite(packet, sizeof(uint8_t), MAX, f_w);
            fflush(f_w);
            if (writeV != MAX)
                return true;
            debug_print("write value %d feof %d, %d data write \n", ferror(f_w), feof(f_w), (int)writeV);
            debug_print("Packet of type %d send\n", packet[0]);
            break;
        }
        FD_ZERO(&set);
    }
    return false;
}
bool recv_packet(uint8_t *packet,
                 int socket, fd_set *set2, FILE *f_r)
{
    struct timeval tv = {1, 0};
    fd_set set;
    int r;
    debug_print("socket %d\n", socket);
    while (true)
    {
        FD_ZERO(&set);
        FD_SET(socket, &set);
        r = select(socket + 1, &set, NULL, NULL, &tv);
        //debug_print("recv %d\n", r);
        if (r == -1)
        {
            return true;
        }
        //debug_print("recv %d\n", r);
        if (FD_ISSET(socket, &set))
        {
            size_t readV;
            readV = fread(packet, sizeof(uint8_t), MAX, f_r);
            debug_print("%d bytes data read\n", (int)readV);
            if (readV != MAX)
            {
                if (readV < 1)
                {
                    errmsgf("Socket disconnect\n");
                }
                return true;
            }
            //fflush(f_r);
            debug_print("Packet type: %d \n", (int)packet[0]);
            break;
        }
        FD_ZERO(&set);
    }
    return false;
}
