//
//  CClient.cpp
//  SocketRVA
//
//  Created by Efrain Astudillo on 2/18/14.
//  Copyright (c) 2014 Efrain Astudillo. All rights reserved.
//

#include "CClient.h"
// ===================  constructores   ========================//
using namespace rva;

std::string CClient::mMessage = "";

CClient::CClient()// : mX(a),mY(b),mD(c),mK(d),mSocket(s),mID(i),mGameOver(go),mTrackerName(""),mRobot(1),mFalseAgents(mf),mThread(t)
{
    mX = rand()%10;
    mY = rand()%10;
    mD = rand()%2;
    mK = 0;
    
    mSocket = 0;
    mID = 0;
    mGameOver = false;
    mTrackerName = "Default";
    mRobot = 1;
    mFalseAgents = NULL;
    mThread = NULL;
}

CClient::CClient(int sockt) : mSocket(sockt){
    mSocket = sockt;
    mX = rand()%10;
    mY = rand()%10;
    mD = rand()%2;
    mK = 0;
    
    mID = 0;
    mGameOver = false;
    mTrackerName = "Default";
    mRobot = 0;
    mFalseAgents = NULL;
    mThread = new std::thread(std::bind(&CClient::run, std::ref(*this)) );
}

CClient::CClient(const CClient& cli){
    mX = cli.mX; mY = cli.mY; mD = cli.mD; mK = cli.mK;
    mSocket = cli.mSocket;
    mID = cli.mID;
    mGameOver = cli.mGameOver;
    mTrackerName = cli.mTrackerName;
    mRobot = cli.mRobot;
    mFalseAgents = cli.mFalseAgents;
    mThread = cli.mThread;
}
CClient::CClient(CClient&& cli){
    mX = cli.mX; mY = cli.mY; mD = cli.mD; mK = cli.mK;
    mSocket = cli.mSocket;
    mID = cli.mID;
    mGameOver = cli.mGameOver;
    mTrackerName = cli.mTrackerName;
    mRobot = cli.mRobot;
    mFalseAgents = cli.mFalseAgents;
    mThread = cli.mThread;

    cli.mX = 0; cli.mY = 0; cli.mD = 0; cli.mK = 0;
    cli.mSocket = 0;
    cli.mID = 0;
    cli.mGameOver = false;
    cli.mTrackerName = "";
    cli.mRobot = 0;
    //cli.mFalseAgents = NULL;
    cli.mThread = NULL;
}
CClient& CClient::operator=(const CClient& cli){
    mX = cli.mX; mY = cli.mY; mD = cli.mD; mK = cli.mK;
    mSocket = cli.mSocket;
    mID = cli.mID;
    mGameOver = cli.mGameOver;
    mTrackerName = cli.mTrackerName;
    mRobot = cli.mRobot;
    mFalseAgents = cli.mFalseAgents;
    mThread = cli.mThread;
    return *this;
}
CClient& CClient::operator=(CClient&& cli){
    mX = cli.mX; mY = cli.mY; mD = cli.mD; mK = cli.mK;
    mSocket = cli.mSocket;
    mID = cli.mID;
    mGameOver = cli.mGameOver;
    mTrackerName = cli.mTrackerName;
    mRobot = cli.mRobot;
    mFalseAgents = cli.mFalseAgents;
    mThread = cli.mThread;
    
    cli.mX = 0; cli.mY = 0; cli.mD = 0; cli.mK = 0;
    cli.mSocket = 0;
    cli.mID = 0;
    cli.mGameOver = false;
    cli.mTrackerName = "";
    cli.mRobot = 0;
    cli.mFalseAgents = NULL;
    cli.mThread = NULL;
    return *this;
}

CClient::~CClient(){
    //close(mSocket);
}
// ===================  constructores   ============================

void CClient::setupVrpn(){
    char connectionName[128];
    int  port = 3883;
    
    sprintf(connectionName,"200.126.23.195:%d", port);
    
    mConnectionVrpn = vrpn_get_connection_by_name(connectionName);
    
    //  debo obtener el nombre del tracker mediandte la red mTrackerName = rcvMsg()
    
    mTracker = new vrpn_Tracker_Remote(mTrackerName.c_str(), mConnectionVrpn);
    
    mTracker->register_change_handler(NULL, handle_vrpn);

}
//========================   handle Vrpn    =========================
void CClient::handle_vrpn(void *userdata,vrpn_TRACKERCB track){
    char buffer[256];
    sprintf(buffer, "%f,%f",track.pos[0],track.pos[1]);
    mMessage = buffer;
}

//========================   thread     =============================
void CClient::run(){
    while (!mGameOver)
    {
        if (mRobot == 1)// si es una persona
        {
            mTracker->mainloop();
            mConnectionVrpn->mainloop();
            usleep(7000);
            
        }else // es un jugador falso
        {
            //mover jugador, actualizar coordenadas
            sleep(1);
            LOG("MSG: "<<getMsg())
        }
        LOG("Estoy en el cliente")
        sleep(5);
    }
    LOG("!!BYE!!")
}
//========================   thread     =============================

long CClient::sendMsg(std::string &msg){
    long len = 0;
    if((len = send(mSocket, msg.c_str(), msg.length(),0)) == -1)
    {
        perror("Error to send data");
        return -1;
    }
    LOG("# bytes enviados : "<<len)
    return len;//cantidad de bytes enviados al cliente.
}

int CClient::rcvMsg(std::string &msg){
    LOG("Estoy recibiendo un mensaje");
    int status = 0;
    int len = 2048;
    char mesage[len];
    
    if((status = recv(mSocket, &mesage, len - 1, 0) == -1)){
        perror("Error receiving message");
        LOG("# error: "<<status)
        sleep(3);
        return -1;
    }
    //mesage[status] = '\0';
    msg = mesage;
    boost::property_tree::ptree p(msg);
    
    return status;  //cantidad de bytes recibidos o -1 (error)
}

//  ======================    getters     ============================
//obtener mensaje para enviarlo por la red
std::string CClient::getMsg(){
    /*boost::property_tree::ptree ptre;
    
    ptre.put("", "");
    std::stringstream ss;
    boost::property_tree::write_json(ss, ptre,false);
    ss.str().c_str();*/
    
    mMessage ="";
    char buffer[1024] = {0};
    std::vector<CClient>::iterator iter = mFalseAgents->begin();
    while (iter != mFalseAgents->end()) {
        sprintf(buffer, "%f,%f,%d" ,iter->mX,iter->mY,iter->mD);
        mMessage.append(buffer);
        iter++;
        LOG(mMessage)
    }
    sprintf(buffer, "%f,%f,%d" ,mX,mY,mD);
    mMessage.append(buffer);
    return mMessage;
}

int CClient::getSocket(){
    return mSocket;
}

void CClient::setAgents(std::vector<CClient> *agents){
    mFalseAgents = agents;
}
