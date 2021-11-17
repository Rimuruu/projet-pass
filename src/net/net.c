#include "net.h"

bool make_sockaddr(struct sockaddr_in *serv_addr,
                   char *ip_addr,
                   int port)
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
        errmsgf("err bind %s\n",strerror(errno));
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
