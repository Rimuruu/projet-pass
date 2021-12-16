#include "net.h"

bool make_sockaddr(struct sockaddr_in *serv_addr,
                   char *ip_addr,uint16_t port)
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
    debug_print("socket debug %d\n",(*s));
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
                 int socket, FILE *f_w)
{
    
    size_t writeV=0;

    
    writeV = fwrite(packet, sizeof(uint8_t), MAX, f_w);

   
    debug_print("%d bytes data write\n", (int)writeV);
    if(writeV != MAX) {
        debug_print("%s\n",strerror(errno));
        return true;
    }
    if(ferror(f_w) != 0){
        debug_print("ferror %s\n",strerror(errno));
        return true;
    }
    fflush(f_w);
    

    return false;
}
bool recv_packet(uint8_t *packet,
                 int socket, FILE *f_r)
{

    size_t readV=0;
    readV = fread(packet, sizeof(uint8_t), MAX, f_r);
    debug_print("%d bytes data read\n", (int)readV);
    if (readV  == 0)
    {
        if(feof(f_r) != 0){
            debug_print("Socket disconnect \n");
            return true;
        }
        if(ferror(f_r) != 0){
            errmsgf("Error read \n");
            return true;
        }
    }
    if(readV > MAX){
        return true;
    }
    fflush(f_r);
    debug_print("Packet type: %d \n", (int)packet[0]);
    
    return false;
}

bool recv_from(uint8_t* packet,
                struct Client_info* clients,int c){
    fd_set set;
    int r;
    uint8_t tmp[MAX] ;
    struct timeval timeout;      
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    int maxfd = clients[0].socket > clients[1].socket ?clients[0].socket : clients[1].socket; 
    debug_print("Waiting client %d\n",c);
    while(true){
        FD_ZERO(&set);
        FD_SET(clients[0].socket,&set);
        FD_SET(clients[1].socket,&set);
        r = select(maxfd+1,&set,NULL,NULL,&timeout);
        if(r == -1){
            return false;
        }
        if(r > 0){
            if(FD_ISSET(clients[0].socket,&set)){
                if(c == 0){
                    if(recv_packet(packet,clients[0].socket,clients[0].f_r)){
                        return true;
                    }
                    return false;
                }else{
                    if(recv_packet(tmp,clients[0].socket,clients[0].f_r)){
                        
                        return true;
                    }
                    debug_print("Didn't ask client\n");
                }
            }
            if(FD_ISSET(clients[1].socket,&set)){
                if(c == 1){
                    if(recv_packet(packet,clients[1].socket,clients[1].f_r)){
                        return true;
                    }
                    return false;
                }else{
                    if(recv_packet(tmp,clients[1].socket,clients[1].f_r)){
                         return true;
                
                    }
                    debug_print("Didn't ask client\n");
                    }
            }
        }
    }

    
}



bool send_to(
            uint8_t* packet,struct Client_info* clients,int c){
    fd_set set;
    int r;
    uint8_t tmp[MAX] ;
    struct timeval timeout;      
    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    int maxfd = clients[0].socket > clients[1].socket ?clients[0].socket : clients[1].socket; 
    FD_ZERO(&set);
    FD_SET(clients[0].socket,&set);
    FD_SET(clients[1].socket,&set);
    r = select(maxfd+1,&set,NULL,NULL,&timeout);
    if(r == -1){
        return false;
    }
    if(r > 0){
        if(FD_ISSET(clients[0].socket,&set)){
          
            if(recv_packet(tmp,clients[0].socket,clients[0].f_r)){
                
                return true;
            }
            debug_print("Didn't ask client\n");
       
        }
        if(FD_ISSET(clients[1].socket,&set)){
            
            if(recv_packet(tmp,clients[1].socket,clients[1].f_r)){
                    return true;
        
            }
            debug_print("Didn't ask client\n");
                
        }
    }

    if(send_packet(packet, clients[c].socket, clients[c].f_w)){
        return true;
    }
    return false;
    

    
}
