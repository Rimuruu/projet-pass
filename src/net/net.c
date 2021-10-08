#include "net.h"


bool make_sockaddr(struct sockaddr_in* serv_addr,
                    char * ip_addr,
                    int port
                    ){
    if(inet_aton(ip_addr,&(serv_addr->sin_addr))==0){

        return true;
    }

    serv_addr->sin_family = AF_INET;
    serv_addr->sin_port = htons(port);

    return false;

}

bool init_socket(int* s){
    (*s) = socket(AF_INET,SOCK_DGRAM,0);
    if((*s) == -1){
        errmsgf("err socket init");
        return true;
    }

    return false;
}


bool init_bind(int* s,struct sockaddr * my_addr){

    if (bind(*s, my_addr,sizeof((*my_addr))) == -1){
        errmsgf("err bind");
        return true;
    }

    return false;


}


bool sendGame(int* s,struct Game* game,struct sockaddr * dest_addr){

    if(sendto(*s,game, sizeof(struct Game), MSG_CONFIRM , dest_addr, sizeof(*dest_addr)) == -1){
        errmsgf("err send%s\n",strerror(errno));    
        return true;
    }
    return false;
}


bool recvGame(int* s,struct Game* game,struct sockaddr * src_addr){
    socklen_t size = sizeof(*src_addr);
    if(recvfrom(*s,game, sizeof(struct Game), MSG_CONFIRM ,  src_addr, &size) == -1){
        errmsgf("err recv %s\n",strerror(errno));       
        return true;
    }
    return false;
}


