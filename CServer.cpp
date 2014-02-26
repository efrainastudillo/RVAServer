//
//  CServer.cpp
//  SocketRVA
//
//  Created by Efrain Astudillo on 2/17/14.
//  Copyright (c) 2014 Efrain Astudillo. All rights reserved.
//

#include "CServer.h"
#include <thread>

using namespace rva;

std::string CServer::mMESSAGE = "";

//========================      contructores        ============================

CServer::CServer():mConn(2014),mPort(2014),mThread(NULL),mMutex(),mStart(false){
    
}

CServer::CServer(int port) : mPort(port),mConn(port),mThread(NULL),mMutex(),mStart(false){
    CClient c;
    for (int i = 0 ; i < 3;  i++) {
        //mAgentes.push_back(c);
    }
}

CServer::CServer(CServer& ser){
    LOG("copy Cserver")
    *this = ser;
}

CServer::CServer(CServer&& ser){
    LOG("move CServer")
    mThread = ser.mThread;
    mPort = ser.mPort;
    mConn = ser.mConn;
    
    ser.mThread = NULL;
    ser.mPort = 0;
    ser.mConn = NULL;
}

CServer& CServer::operator=(CServer& ser){
    LOG("assignament Cserver")
    mThread = ser.mThread;
    mPort = ser.mPort;
    mConn = ser.mConn;
    return *this;
}

CServer& CServer::operator=(CServer&& ser){
    LOG("move assignament Cserver")
    mThread = ser.mThread;
    mPort = ser.mPort;
    mConn = ser.mConn;
    
    ser.mThread = NULL;
    ser.mPort = 0;
    ser.mConn = NULL;
    return *this;
}

CServer::~CServer(){
    delete [] mThread;
}

//========================      contructores        ============================

void CServer::start(){
    mThread = new std::thread(std::bind(&CServer::run, std::ref(*this)) );
}


// thread para obtener a cada cliente en la comunicacion por la red
void CServer::run(){
    int index = 0;
    while (!mStart) {
        LOG("Wait for client")
        int client_fd = waitForClient();

        CClient client(client_fd);
        clientes.insert(std::make_pair(index, std::move(client)));
        std::string msg;
        clientes.find(0)->second.rcvMsg(msg);
        
        //boost::property_tree::ptree p;
        //std::stringstream ss;
        //ss << msg;
        //boost::property_tree::read_json(ss, p);
        //LOG(p.get<std::string>("modo_juego"));
        LOG(msg)
        mStart = true;
        LOG(client_fd)
        index ++;
        //clientes.find(0)->second.sendMsg(mMESSAGE);
    }
    
    while (true) {
        std::srand((int)std::time(NULL));
        MOVER(mAgentes);//
        std::string msg;
        BUILD_MESSAGE_AGENTS(mAgentes, msg);
        LOG(msg)
        std::map<int,CClient>::iterator iter = clientes.begin();
        while (iter != clientes.end())
        {
            if(iter->second.sendMsg(msg) == -1){
                perror("Error sending message");
                break;
            }
            //std::string m;
            //iter->second.rcvMsg(m);
            iter++;
        }
        LOG(" = WAITING TO FINISH =")
    }
}

// configuracion inicial del socket para la comunicacion (socket, bind,  listen)
void CServer::connect(){
    rva::CClient c{};
    for (int i = 0 ; i < 3;  i++) {
        mAgentes.push_back(c);
    }
    mConn.initialize();
    LOG(" = Configured = ")
}

// se bloquea esperando una conexion de un cliente utilzando el socket
int CServer::waitForClient(){
    return std::move(mConn.acceptConnection());
}

void CServer::joinThread(){
    mThread->join();
}