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
//CGame*                      CGame::mInstance = NULL;
boost::mutex                CGame::mMutexSingleton;
//bool                        CGame::mIniciarJuego = false;
const int                   CGame::CANTIDAD_ESPIAS_MAX = 2;
int                         CGame::mCantidadEspias = 0;
//std::string                 CGame::mMessage = "";
boost::mutex                CGame::mMutex;
boost::mutex                CGame::mMutexLog;
std::vector<rva::CClient>   CGame::mAgentes;
std::map< int, CClient>     CGame::clientes;

CGame::CGame(){
    CGame::mIniciarJuego = false;
    CGame::cantidadJugadoresFalsos = 3;
}

CGame &CGame::getInstance(){
    std::call_once( CGame::only_one,[] ()
       {
           CGame::mInstance.reset( new CGame() );
           
           std::cout << "safeSingleton::create_singleton_() | thread id " << std::endl;
       });
    
    /*if (mInstance == NULL) {
        mMutexSingleton.lock();
        if (mInstance == NULL) {
            mInstance = new CGame();
        }
        mMutexSingleton.unlock();
    }
     */
    return *CGame::mInstance;
}

void CGame::changeState(int id, std::vector<rva::CClient> &mAgentes, std::map<int, CClient> &clientes){
    std::vector<CClient>::iterator it = mAgentes.begin();
    while (it != mAgentes.end()) {
        if (it->mID == id) {
            it->mActivo = 0;
            CGame::getInstance().cantidadJugadoresFalsos--;
            return;
        }
        it++;
    }
    
    std::map<int,rva::CClient>::iterator it1 = clientes.begin();
    while (it1 != clientes.end()) {
        if (it1->second.mID == id) {
            it1->second.mActivo = 0;
            CGame::getInstance().mCantidadEspias--;
            return;
        }
        it1++;
    }
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