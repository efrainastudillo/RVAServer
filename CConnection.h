//
//  CConnection.h
//  SocketRVA
//
//  Created by Efrain Astudillo on 2/17/14.
//  Copyright (c) 2014 Efrain Astudillo. All rights reserved.
//

#ifndef __SocketRVA__CConnection__
#define __SocketRVA__CConnection__

#include "CUtil.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>

namespace rva{
    
    class CConnection{
    public:
        CConnection();
        CConnection(int port);
        CConnection(CConnection& conn);
        CConnection(CConnection&& conn);
        
        CConnection& operator=(CConnection& conn);
        CConnection& operator=(CConnection&& conn);
        
        ~CConnection();
        
        int getSocket();
        void initialize();
        int acceptConnection();
        
        
    private:
        std::string mIP;
        int mPortNumber;		//Socket port number
        int mSocketFd;          //Socket file descriptor
        
        int mBlocking;			//blocking flag
        int mBind;              //Binding flag

    };
    
}
#endif /* defined(__SocketRVA__CConnection__) */
