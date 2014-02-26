//
//  CConnection.cpp
//  SocketRVA
//
//  Created by Efrain Astudillo on 2/17/14.
//  Copyright (c) 2014 Efrain Astudillo. All rights reserved.
//

#include "CConnection.h"
#include <sstream>
using namespace rva;

CConnection::CConnection() :
mBind(0),mBlocking(0),mPortNumber(0),mSocketFd(0)
{

}

CConnection::CConnection(int port) :mIP("127.0.0.1"), mPortNumber(port),mBind(0),mBlocking(0),mSocketFd(0){}
CConnection::CConnection(CConnection& conn){
    LOG("copy CConnection")
    *this = conn;
}
CConnection::CConnection(CConnection&& conn){
    LOG("moving CConnection")
    *this = conn;
}

CConnection& CConnection::operator=(CConnection& conn){
    LOG("assignament CConnection")
    mSocketFd   = conn.mSocketFd;
    mPortNumber = conn.mPortNumber;
    mIP         = conn.mIP;
    
    return *this;
}
CConnection& CConnection::operator=(CConnection&& conn){
    LOG("move assigment CConnection")
    mSocketFd   = conn.mSocketFd;
    mPortNumber = conn.mPortNumber;
    mIP         = conn.mIP;
    
    conn.mIP = "";
    conn.mPortNumber = 0;
    conn.mSocketFd = 0;
    return *this;
}

CConnection::~CConnection(){}

void CConnection::initialize(){

    int status;
    
    //struct addrinfo hints;
    addrinfo hints;
    addrinfo *servinfo, *p;
    
    //struct addrinfo *servinfo; // will point to the results
    memset(&hints, 0, sizeof hints); // make sure the struct is empty
    
    hints.ai_family = AF_UNSPEC; // don't care IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
    hints.ai_flags = AI_PASSIVE; // fill in my IP for me
    
    std::stringstream ss;
    ss << mPortNumber;
    std::string str = ss.str();
    
    if ((status = getaddrinfo(NULL,str.c_str() , &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
        exit(1);
    }
    int yes=1;
    //char yes='1'; // Solaris people use this
    
    for(p = servinfo; p != NULL; p = p->ai_next){
        
        if ((mSocketFd = socket(p->ai_family,p->ai_socktype,p->ai_protocol)) == -1)
        {
            perror("Error socket");
        }
        // lose the pesky "Address already in use" error message
        if (setsockopt(mSocketFd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }
        
        if ( (status = bind(mSocketFd, p->ai_addr, p->ai_addrlen)) == -1) {
            close(mSocketFd);
            perror("Error bind");
            continue;
        }
        
        break;
    }
    
    
    // servinfo now points to a linked list of 1 or more struct addrinfos
    // ... do everything until you don't need servinfo anymore ....
    freeaddrinfo(servinfo); // free the linked-list

    if ((status = listen(mSocketFd, 8)) == -1 ) {
        perror("Error listen");
        exit(EXIT_FAILURE);
    }
}

int CConnection::acceptConnection(){
    char ipstr[INET6_ADDRSTRLEN];
    socklen_t sin_size;
    struct sockaddr_storage their_addr;
    sin_size = sizeof their_addr;
    int new_fd = accept(mSocketFd, (struct sockaddr *)&their_addr, &sin_size);
    
    void* var;
    if(their_addr.ss_family == AF_INET)
    {
        var = &(((struct sockaddr_in*)(struct sockaddr *)&their_addr)->sin_addr);
    }else{
        var = &(((struct sockaddr_in6*)(struct sockaddr *)&their_addr)->sin6_addr);
    }
    
    inet_ntop(their_addr.ss_family, var, ipstr, sizeof ipstr);
    
    printf("Server: received connection from %s\n", ipstr);
    LOG("Client connected : "<<new_fd)
    
    return new_fd;
}
int CConnection::getSocket(){
    return mSocketFd;
}