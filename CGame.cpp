//
//  CGame.cpp
//  SocketRVA
//
//  Created by Efrain Astudillo on 3/3/14.
//  Copyright (c) 2014 Efrain Astudillo. All rights reserved.
//

#include "CClient.h"

using namespace rva;

std::once_flag              CGame::only_one;
std::shared_ptr< CGame >    CGame::mInstance = nullptr;
bool                        CGame::mIniciarJuego = false;
const int                   CGame::CANTIDAD_ESPIAS_MAX = 3;
int                         CGame::mCantidadEspias = 0;
std::string                 CGame::mMessage = "";
boost::mutex                CGame::mMutex;
boost::mutex                CGame::mMutexLog;

CGame::CGame(){}

CGame &CGame::getInstance(){
    std::call_once( CGame::only_one,[] ()
       {
           CGame::mInstance.reset( new CGame() );
           
           std::cout << "safeSingleton::create_singleton_() | thread id " << std::endl;
       });
    
    return *CGame::mInstance;
}

void CGame::lock(){
    CGame::getInstance().mMutex.lock();
}
void CGame::unlock(){
    CGame::getInstance().mMutex.unlock();
}

void CGame::unlockLog(){
    CGame::getInstance().mMutexLog.unlock();
}
void CGame::lockLog(){
    CGame::getInstance().mMutexLog.lock();
}