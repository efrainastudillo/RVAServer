//
//  CServer.h
//  SocketRVA
//
//  Created by Efrain Astudillo on 2/17/14.
//  Copyright (c) 2014 Efrain Astudillo. All rights reserved.
//

#ifndef __SocketRVA__CServer__
#define __SocketRVA__CServer__

#include <iostream>
#include "vrpn_Connection.h"
#include "vrpn_Tracker.h"
#include "CClient.h"
#include "CConnection.h"
#include <thread>
#include <map>
#include <list>

namespace rva {

    class CServer{
    public:
        CServer();
        CServer(int port);
        CServer(CServer& ser);
        CServer(CServer&& ser);
        CServer& operator=(CServer&);
        CServer& operator=(CServer&&);
        ~CServer();
        
        void connect();
        int waitForClient();
        void start();
        void run();
        void joinThread();
        
        void print(){std::cout<<"Posrt: "<<mPort<<std::endl<<"Socket: "<<mConn.getSocket()<<std::endl;};
        std::list<CClient> cli;
        std::vector<rva::CClient> mAgentes;
        
    private:
        std::thread * mThread;
        boost::mutex mMutex;
        rva::CConnection mConn;
        static std::string mMESSAGE;
        int mPort;
        
        std::map< int, CClient> clientes;
        
        
        bool mStart;
    };
    
};
#endif /* defined(__SocketRVA__CServer__) */
