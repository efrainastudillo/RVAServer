//
//  CClient.cpp
//  SocketRVA
//
//  Created by Efrain Astudillo on 2/18/14.
//  Copyright (c) 2014 Efrain Astudillo. All rights reserved.
//

#include "CClient.h"
#include "boost/property_tree/xml_parser.hpp"
// ===================  constructores   ========================//
using namespace rva;


std::string CClient::mMessage = "";
float CClient::mX = 0;
float CClient::mY = 0;
float CClient::mZ = 0;
CClient::CClient()// : mX(a),mY(b),mD(c),mK(d),mSocket(s),mID(i),mGameOver(go),mTrackerName(""),mRobot(1),mFalseAgents(mf),mThread(t)
{
    mX = rand()%10;
    mY = rand()%10;
    mD = rand()%2;
    mK = 0;
    mZ = rand()%10;
    mActivo = 1;
    mInicio = false;
    
    mSocket = 0;
    mID = 0;
    mGameOver = false;
    mTrackerName = "Default";
    mTypeClient = 1;//robot
    mFalseAgents = NULL;
    mThread = NULL;
}

CClient::CClient(int sockt,int mtype_client) : mSocket(sockt){
    mSocket = sockt;
    mX = rand()%10;
    mY = rand()%10;
    mD = rand()%2;
    mK = 0;
    mZ = rand()%10;
    mActivo = 1;
    mInicio = false;
    
    mID = 0;
    mGameOver = false;
    mTrackerName = "Default";
    mTypeClient = mtype_client;// 0 - es un espia, 1 - es un robot, 2 es un detective
    mFalseAgents = NULL;
    
    // Put the socket in non-blocking mode:
    if(fcntl(mSocket, F_SETFL, fcntl(mSocket, F_GETFL) | O_NONBLOCK) < 0) {
        perror("Error bloking socket");
    }
    
    mThread = new std::thread(std::bind(&CClient::run, std::ref(*this)) );
}

CClient::CClient(const CClient& cli){
    mX = cli.mX; mY = cli.mY; mD = cli.mD; mK = cli.mK;mZ = cli.mZ; mActivo = cli.mActivo;
    mSocket = cli.mSocket;
    mID = cli.mID;
    mGameOver = cli.mGameOver;
    mTrackerName = cli.mTrackerName;
    mTypeClient = cli.mTypeClient;
    mFalseAgents = cli.mFalseAgents;
    mThread = cli.mThread;
    mInicio = cli.mInicio;
}
CClient::CClient(CClient&& cli){
    mX = cli.mX; mY = cli.mY; mD = cli.mD; mK = cli.mK; mZ = cli.mZ; mActivo = cli.mActivo;
    mSocket = cli.mSocket;
    mID = cli.mID;
    mGameOver = cli.mGameOver;
    mTrackerName = cli.mTrackerName;
    mTypeClient = cli.mTypeClient;
    mFalseAgents = cli.mFalseAgents;
    mThread = cli.mThread;
    mInicio = cli.mInicio;

    cli.mX = 0; cli.mY = 0; cli.mD = 0; cli.mK = 0;cli.mZ = 0;cli.mActivo = 0;
    cli.mSocket = 0;
    cli.mID = 0;
    cli.mGameOver = false;
    cli.mTrackerName = "";
    cli.mTypeClient = 0;
    //cli.mFalseAgents = NULL;
    cli.mThread = NULL;
}
CClient& CClient::operator=(const CClient& cli){
    mX = cli.mX; mY = cli.mY; mD = cli.mD; mK = cli.mK;mZ = cli.mZ; mActivo = cli.mActivo;
    mSocket = cli.mSocket;
    mID = cli.mID;
    mGameOver = cli.mGameOver;
    mTrackerName = cli.mTrackerName;
    mTypeClient = cli.mTypeClient;
    mFalseAgents = cli.mFalseAgents;
    mThread = cli.mThread;
    mInicio = cli.mInicio;
    
    return *this;
}
CClient& CClient::operator=(CClient&& cli){
    mX = cli.mX; mY = cli.mY; mD = cli.mD; mK = cli.mK;mZ = cli.mZ; mActivo = cli.mActivo;
    mSocket = cli.mSocket;
    mID = cli.mID;
    mGameOver = cli.mGameOver;
    mTrackerName = cli.mTrackerName;
    mTypeClient = cli.mTypeClient;
    mFalseAgents = cli.mFalseAgents;
    mThread = cli.mThread;
    
    cli.mX = 0; cli.mY = 0; cli.mD = 0; cli.mK = 0;cli.mZ = 0;cli.mActivo = 0;
    cli.mSocket = 0;
    cli.mID = 0;
    cli.mGameOver = false;
    cli.mTrackerName = "";
    cli.mTypeClient = 0;
    cli.mFalseAgents = NULL;
    cli.mThread = NULL;
    return *this;
}

CClient::~CClient(){
    //close(mSocket);
}
// ===================  constructores   ============================

void CClient::setupVrpn(std::string & name){
    mTrackerName = name;
    char connectionName[128];
    int  port = 3883;
    std::string ip = "";
    
    try {
        boost::property_tree::ptree p;
        std::stringstream ss;
        ss << "vrpn.xml";
        boost::property_tree::read_xml("vrpn.xml", p);
        ip = p.get<std::string>("vrpn.ip");
        port = p.get<int>("vrpn.port");
        LOG("[VRPN:IP] "<<ip)
        LOG("[VRPN:PORT] "<<port)
        
    } catch (boost::property_tree::ptree_error &e) {
        LOG("[setup vrpn] "<<e.what())
    }
    
    ip.append(":" + std::to_string(port));
    LOG("[VRPN:FORMAT] "<<ip)
    sprintf(connectionName,ip.c_str(),NULL);

    mConnectionVrpn = vrpn_get_connection_by_name(connectionName);
    mTracker = new vrpn_Tracker_Remote(mTrackerName.c_str(), mConnectionVrpn);
    mTracker->register_change_handler(NULL, handle_vrpn);
}
//========================   handle Vrpn    =========================
// Aqui se obtiene los datos que envia el VRPN
void CClient::handle_vrpn(void *userdata,vrpn_TRACKERCB track){
    mX = track.pos[0];
    mY = track.pos[2];
}

//========================   thread     =============================
void CClient::run(){
    std::string msg;
    long bytes = 0;

    while (!mGameOver)
    {
        bytes = rcvMsg(msg);
        if (bytes > 0)
        {
            parserMessage(msg);
            bytes = sendMsg(msg);
        }
        else
        {
            continue;
        }
        
        if (mTypeClient == ES_ESPIA)
        {
            //mInicio = true cuando el detective envia el msg de inicio de juego
            // && CGame::getInstance().iniciar_juego == true
            if (CGame::getInstance().mIniciarJuego) {
                mTracker->mainloop();
                mConnectionVrpn->mainloop();
                //enviar coordenadas

            }else
            {
                
            }
        }else if (mTypeClient == ES_DETECTIVE)
        {
            LOG("Soy detective")
        }
        
        CGame::getInstance().lock();
        CGame::getInstance().mMessage = "Hello world probando mutex";
        CGame::getInstance().unlock();
        
        CGame::getInstance().lockLog();
        LOG(CGame::getInstance().mMessage)
        LOG("Estoy en el cliente")
        CGame::getInstance().unlockLog();
        
    }
    LOG("!!BYE!!")
}
//========================   thread     ==============================
// send a message to user through socket and                        //
// receive how many bytes you sent                                  //
//////////////////////////////////////////////////////////////////////
long CClient::sendMsg(std::string &msg){
    long len = 0;
    if((len = send(mSocket, msg.c_str(), msg.length(),0)) == -1)
    {
        perror("Error to send data");
        return -1;
    }
    CGame::getInstance().lockLog();
    LOG("# bytes enviados : "<<len)
    CGame::getInstance().unlockLog();
    return len;//cantidad de bytes enviados al cliente.
}
//====================================================================
// return number of bytes received on mesage                        //
// and data is contained on msg                                     //
//////////////////////////////////////////////////////////////////////
long CClient::rcvMsg(std::string &msg){
    long len = 0;
    int size = 512;
    char mesage[size];
    
    if((len = recv(mSocket, &mesage, size - 1, 0)) == -1){
        perror("Error receiving message");
        return -1;
    }
    mesage[len] = '\0';
    msg = mesage;
    //boost::property_tree::ptree p(msg);
    
    return len;  //cantidad de bytes recibidos o -1 (error)
}
// ==================================================================
// Parser message
/////////////////////////////////////////////////////////////////////
void CClient::parserMessage(std::string & msg){
    int type_msg;
    try {
        boost::property_tree::ptree p;
        std::stringstream ss;
        ss << msg;
        boost::property_tree::read_json(ss, p);
         type_msg = p.get<int>("tipo_mensaje");
        
        if (type_msg == 0)
        {
            int modo = p.get<int>("modo_juego");
            if (modo == 0) {//detective
                mTypeClient = ES_DETECTIVE;
            }
            else //espia
            {
                mTrackerName = p.get<std::string>("nombre_tracker");
                setupVrpn(mTrackerName);
                CGame::getInstance().lockLog();
                mTypeClient = ES_ESPIA;
                LOG(mTrackerName)
                LOG(p.get<std::string>("modo_juego"))
                CGame::getInstance().unlockLog();
            }
            
        }else if (type_msg == 1)
        {
            int ej = p.get<int>("estado_juego");
            if (ej == 1)
            {
                CGame::getInstance().mIniciarJuego = true;
                //mInicio = true;
            }else
            {
                mGameOver = true;
            }
        }
    } catch (boost::property_tree::ptree_error &e) {
        CGame::getInstance().lockLog();
        LOG("[ parserMessage ] "<<e.what())
        CGame::getInstance().unlockLog();
    }
    
     boost::property_tree::ptree ptre;  
    if (type_msg == 0)
        {   
        ptre.put("tipo_mensaje", 0);
        ptre.put("estado", 1);// si es efectivo
        ptre.put("id_jugador", mID);
    } else if (type_msg == 1){
        ptre.put("inicio", 1);
    }
    
     std::stringstream ss2;
     boost::property_tree::write_json(ss2, ptre,false);
    msg = ss2.str();
}

//  ======================    getters     ============================
//obtener mensaje para enviarlo por la red
std::string CClient::getMsg(){
   /* boost::property_tree::ptree ptre;
    
    ptre.put("", "");
    std::stringstream ss;
    boost::property_tree::write_json(ss, ptre,false);
    ss.str().c_str();
    */
    //mMessage = getMsgRobots();

    return getMsgRobots();
}

std::string CClient::getMsgRobots(){
    return "";
}

int CClient::getSocket(){
    return mSocket;
}

void CClient::setNameTracker(std::string msg){
    mTrackerName = msg;
}
