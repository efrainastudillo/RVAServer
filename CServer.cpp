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
    int index = 3; //El id de 0 - 2 son los falsos
    while (CGame::getInstance().mCantidadEspias < CGame::getInstance().CANTIDAD_ESPIAS_MAX) {
        LOG("Wait for client")
        int client_fd = waitForClient();
        CClient client(client_fd);
        client.mID = index;
        clientes.insert(std::make_pair(index, client));
        index++;
        CGame::getInstance().mCantidadEspias++;
    }
    
    LOG("Wait for Detective")
    int client_fd = waitForClient();
    CClient client(client_fd,2);//detective
    client.mID = index;
    clientes.insert(std::make_pair(index, client));


    while (!mStart) {
        if(!CGame::getInstance().mIniciarJuego){
           // continue;
        }
        std::srand((int)std::time(0));
        MOVER(mAgentes);//
        std::string msg;
        //BUILD_MESSAGE_AGENTS(mAgentes, clientes);

        CGame::getInstance().lockLog();
        LOG(" = WAITING TO FINISH =")
        CGame::getInstance().unlockLog();
    }
}

// configuracion inicial del socket para la comunicacion (socket, bind,  listen)
void CServer::connect(){
    // inicializacion de los agentes falsos
    rva::CClient c{};
    
    for (int i = 0 ; i < 3;  i++) {
        c.mID = i;
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