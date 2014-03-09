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
    while (CGame::getInstance().mCantidadEspias < 1/*CGame::getInstance().CANTIDAD_ESPIAS_MAX*/) {
        LOG("Wait for client")
        int client_fd = waitForClient();
        CClient client{client_fd};
        client.mID = index;
        sleep(1);
        if (client.mTypeClient == ES_ESPIA) {
            CGame::getInstance().clientes.insert(std::make_pair(index, client));
            //client = NULL;
            index++;
            CGame::getInstance().mCantidadEspias++;
        }else
        {
            //if detective ya existe continue;
            CClient client2{client_fd,2};//detective
            CGame::getInstance().clientes.insert(std::make_pair(50, client));
            //client = NULL;
            //CClient client(client_fd,2);//detective
        }
        break;
    }
 LOG(" = SALI DEL THREAD")

    // posiciones aleatorias iniciales

    int i;

    srand(time(NULL));

    for (i=0;i<CGame::getInstance().mAgentes.size();i++){
        CGame::getInstance().mAgentes[i].mX = rand()%900 + 100;
        CGame::getInstance().mAgentes[i].mY = rand()%500;
    }

    while (!mStart) {
        if(!CGame::getInstance().mIniciarJuego){
            continue;
        }
        boost::property_tree::ptree p;
        std::srand((int)std::time(0));
        MOVER(CGame::getInstance().mAgentes);
        usleep(100);//
        BUILD_MESSAGE_AGENTS(CGame::getInstance().mAgentes, CGame::getInstance().clientes);

        //LOG(" = WAITING TO FINISH =")
    }
}

// configuracion inicial del socket para la comunicacion (socket, bind,  listen)
void CServer::connect(){
    // inicializacion de los agentes falsos
    rva::CClient c{};
    
    for (int i = 0 ; i < 3;  i++) {
        c.mID = i;
        CGame::getInstance().mAgentes.push_back(c);
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