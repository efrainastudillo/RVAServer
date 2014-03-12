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
    _coordinates = NULL;
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
    _coordinates = new COORDINATES;
    
    mID = 0;
    mGameOver = false;
    mTrackerName = "Default";
    mTypeClient = mtype_client;// 0 - es un espia, 1 - es un robot, 2 es un detective
    mFalseAgents = NULL;
    
    // Put the socket in non-blocking mode:
    if(fcntl(mSocket, F_SETFL, fcntl(mSocket, F_GETFL) | O_NONBLOCK) < 0) {
        perror("Error bloking socket");
    }
    mThread = std::make_shared<std::thread>(std::bind(&CClient::run, std::ref(*this)));
   //mThread = new std::thread(std::bind(&CClient::run, std::ref(*this)) );
}

CClient::CClient(const CClient& cli){
    LOG("Copy Constructor")
    mX = cli.mX; mY = cli.mY; mD = cli.mD; mK = cli.mK;mZ = cli.mZ; mActivo = cli.mActivo;
    mSocket = cli.mSocket;
    mID = cli.mID;
    mGameOver = cli.mGameOver;
    mTrackerName = cli.mTrackerName;
    mTypeClient = cli.mTypeClient;
    mFalseAgents = cli.mFalseAgents;
    mThread = cli.mThread;
    mInicio = cli.mInicio;
    _coordinates = cli._coordinates;
    
}
CClient::CClient(CClient&& cli){
    LOG("MOVE Constructor")
    mX = cli.mX; mY = cli.mY; mD = cli.mD; mK = cli.mK; mZ = cli.mZ; mActivo = cli.mActivo;
    mSocket = cli.mSocket;
    mID = cli.mID;
    mGameOver = cli.mGameOver;
    mTrackerName = cli.mTrackerName;
    mTypeClient = cli.mTypeClient;
    mFalseAgents = cli.mFalseAgents;
    mThread = cli.mThread;
    mInicio = cli.mInicio;
    _coordinates = cli._coordinates;

    cli.mX = 0; cli.mY = 0; cli.mD = 0; cli.mK = 0;cli.mZ = 0;cli.mActivo = 0;
    cli.mSocket = 0;
    cli.mID = 0;
    cli.mGameOver = false;
    cli.mTrackerName = "";
    cli.mTypeClient = 0;
    cli.mFalseAgents = NULL;
    //cli.mThread = NULL;
    cli._coordinates = NULL;
}
CClient& CClient::operator=(const CClient& cli){
    LOG("copy assigment Constructor")
    mX = cli.mX; mY = cli.mY; mD = cli.mD; mK = cli.mK;mZ = cli.mZ; mActivo = cli.mActivo;
    mSocket = cli.mSocket;
    mID = cli.mID;
    mGameOver = cli.mGameOver;
    mTrackerName = cli.mTrackerName;
    mTypeClient = cli.mTypeClient;
    mFalseAgents = cli.mFalseAgents;
    mThread = cli.mThread;
    mInicio = cli.mInicio;
    _coordinates = cli._coordinates;
    return *this;
}
CClient& CClient::operator=(CClient&& cli){
    LOG("MOVE assignment Constructor")
    mX = cli.mX; mY = cli.mY; mD = cli.mD; mK = cli.mK;mZ = cli.mZ; mActivo = cli.mActivo;
    mSocket = cli.mSocket;
    mID = cli.mID;
    mGameOver = cli.mGameOver;
    mTrackerName = cli.mTrackerName;
    mTypeClient = cli.mTypeClient;
    mFalseAgents = cli.mFalseAgents;
    mThread = cli.mThread;
    _coordinates = cli._coordinates;
    
    cli.mX = 0; cli.mY = 0; cli.mD = 0; cli.mK = 0;cli.mZ = 0;cli.mActivo = 0;
    cli.mSocket = 0;
    cli.mID = 0;
    cli.mGameOver = false;
    cli.mTrackerName = "";
    cli.mTypeClient = 0;
    cli.mFalseAgents = NULL;
    //cli.mThread = NULL;
    cli._coordinates = NULL;
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

    mTracker->register_change_handler(_coordinates, handle_vrpn);

}
//========================   handle Vrpn    =========================
// Aqui se obtiene los datos que envia el VRPN
void CClient::handle_vrpn(void *userdata,vrpn_TRACKERCB track){
    //LOG("Coordenadas => "<<pos[0]<<" , "<<pos[1])
    (*(COORDINATES *)userdata)._x = track.pos[0];
    (*(COORDINATES *)userdata)._y = track.pos[2];
     (*(COORDINATES *)userdata)._z = track.pos[1];

}

//========================   thread     =============================
void CClient::run(){
    std::string msg;
    long bytes = 0;

    while (!mGameOver)
    {
        /*
        CGame::getInstance().lockLog();
        LOG(""<<this->mThread->get_id())
        CGame::getInstance().unlockLog();
        sleep(3);
        */
        bytes = rcvMsg(msg);
        if (bytes > 0)
        {
            std::string m = parserMessage(msg);
            bytes = sendMsg(m);
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
            }else
            {
                
            }
        }else if (mTypeClient == ES_DETECTIVE)
        {
            LOG("Soy detective")
        }
        
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
    //CGame::getInstance().lockLog();
    //LOG("# bytes enviados : "<<len)
    //CGame::getInstance().unlockLog();
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
        //perror("Error receiving message");
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
std::string CClient::parserMessage(std::string & msg){
    int type_msg;
    boost::property_tree::ptree p = boost::property_tree::ptree();
    try {
        LOG("Nombre Traker:"<<mTrackerName<<" "<<std::endl<<CGame::getInstance().mCantidadEspias)
        std::stringstream ss ;
        //std::stringstream ss1;
        ss<<msg;
         if(numMensaje!=2){
            boost::property_tree::read_json(ss, p);
            type_msg = p.get<int>("tipo_mensaje");
        }else{
            type_msg=2;
        }
        
        
        if (type_msg == 0)
        {
            numMensaje++;
            int modo = p.get<int>("modo_juego");
            LOG("MODO juego: "<<modo)
            if (modo == 0) {//detective
                mTypeClient = ES_DETECTIVE;
            }
            else //espia
            {
                mTrackerName = p.get<std::string>("nombre_tracker");
                LOG("tracker name : "<<mTrackerName)
                setupVrpn(mTrackerName);
                mTypeClient = ES_ESPIA;
            }
            
        }else if (type_msg == 1)
        {
            numMensaje++;
            //int ej = p.get<int>("estado_juego");
            //if (ej == 1)
            //{
                //CGame::getInstance().mIniciarJuego = true;
            //}else
            //{
                //mGameOver = true;
            //}
        }else if (type_msg == 2)
        {
            //CGame::getInstance().mIniciarJuego = true;
            //aqui procesar los seleccionados

            
        }
    } catch (boost::property_tree::ptree_error &e) {
        CGame::getInstance().lockLog();
        LOG("[ parserMessage ] "<<e.what())
        CGame::getInstance().unlockLog();
    } catch (std::exception &e){
        LOG(" [ parser Message ] "<<e.what())
    }
    
     boost::property_tree::ptree ptre;  
    if (type_msg == 0)
    {
        ptre.put("tipo_mensaje", 0);
        ptre.put("estado", 1);// si el tracker no esta repetido
        ptre.put("id_jugador", mID);
        
    } else if (type_msg == 1)
    {
        while (!CGame::getInstance().mIniciarJuego) {
            continue;
        }
        ptre.put("inicio", 1);
        
    }else if (type_msg == 2)
    {   int id = -1;
        //si es -1
        LOG(msg)
        id=atoi(msg.c_str());

        CGame::getInstance().changeState(id, CGame::getInstance().mAgentes, CGame::getInstance().clientes);
        //gano detective
        if (CGame::getInstance().mCantidadEspias == 0) {
            ptre.put("tipo_mensaje", 3);
            ptre.put("estado", 1);// si es efectivo
        }
        //perdio detective
        else if(CGame::getInstance().cantidadJugadoresFalsos == 0){
            ptre.put("tipo_mensaje", 3);
            ptre.put("estado", 0);// si es efectivo
        }
        else
        {
            //LOG("esta es la respuesta: "<<CGame::getInstance().mMessage);
            msg = CGame::getInstance().mMessage;
            //LOG("ENVIANDO")
            return CGame::getInstance().mMessage;
        }
    }
    
    std::stringstream ss2;
    boost::property_tree::write_json(ss2, ptre,false);
    return ss2.str();
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