//
//  CClient.h
//  SocketRVA
//
//  Created by Efrain Astudillo on 2/18/14.
//  Copyright (c) 2014 Efrain Astudillo. All rights reserved.
//

#ifndef __SocketRVA__CClient__
#define __SocketRVA__CClient__

#include <iostream>
#include <vector>
#include <thread>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "CConnection.h"

#include "vrpn_Connection.h"
#include "vrpn_Tracker.h"


#include "boost/property_tree/ptree.hpp"
#include "boost/property_tree/json_parser.hpp"
#include "boost/thread.hpp"



namespace rva {
    
    enum{ES_ESPIA = 0, ES_ROBOT ,ES_DETECTIVE};
    
    class CClient{
    public:
        
        CClient();
        CClient(int sockt,int mtype_client = 0);

        CClient(const CClient& cli);
        CClient(CClient&& cli);
        CClient& operator=(const CClient& cli);
        CClient& operator=(CClient&& cli);
        
        ~CClient();
        
        //  comunication with clients
        long sendMsg(std::string& msg);
        long rcvMsg(std::string& msg);
        
        // get data coordinates and thread
        static void handle_vrpn(void *userdata, vrpn_TRACKERCB track);
        void run();

        int getSocket();
        
        void setupVrpn(std::string&);
        std::string getMsg();
        std::string getMsgRobots();
        void parserMessage(std::string&);
        void setNameTracker(std::string);
        
    private:
        std::thread *mThread;
        vrpn_Tracker_Remote *   mTracker;
        vrpn_Connection *       mConnectionVrpn;
        
        int mSocket;
        static std::string mMessage;
        
        boost::mutex mMutex;
        
        std::vector<CClient>* mFalseAgents;
        
        
        std::string mTrackerName;
        
    public:
        int mID;
        static float mX;
        static float mY;
        static float mZ;
        int mD;
        int mK;
        int mActivo;
        int mTypeClient;
        // flags to manipulate the logic game
    private:
        bool mGameOver;
        
        bool mInicio;
        
    };
    
// ===========================================================================

    class CGame{
    public:
        CGame();
        ~CGame(){};
        
        static CGame &getInstance();
        void lockLog();
        void unlockLog();
        
        void lock();
        void unlock();
        
        static boost::mutex mMutex;
        static boost::mutex mMutexLog;
        
        static std::string mMessage;
        
        //static std::string
        
        static bool mIniciarJuego;
        static int mCantidadEspias;
        static const int CANTIDAD_ESPIAS_MAX;
    private:
        static boost::mutex mMutexSingleton;
        static CGame* mInstance;
        //static std::shared_ptr<CGame> mInstance;
        //static std::once_flag           only_one;
    };

}

// ===========================================================================


inline static void MOVER(std::vector<rva::CClient>& agentes){
    int i,j,m;
	for (i=0;i<agentes.size();i++){
		if (agentes[i].mK==0){
			m = rand()%10;
			if (m<7){
				agentes[i].mX = agentes[i].mX + (-1 * rand()%2) + (agentes[i].mD*rand()%2 || rand()%2);
				agentes[i].mY = agentes[i].mY + (-1 * rand()%2) + (agentes[i].mD*rand()%2 || rand()%2);
				for (j=0;j<agentes.size();j++){
					if (j!=i){
						if (agentes[j].mX == agentes[i].mX && agentes[j].mY == agentes[i].mY){
							agentes[i].mX = agentes[i].mX+(rand()%2);
							if (rand()%10 > 7)
								agentes[i].mY = agentes[i].mY+1;
							else
								agentes[i].mY = agentes[i].mY-1;
							if (agentes[i].mD == 0)
								agentes[i].mD = 1;
							else
								agentes[i].mD = 0;
						}
					}
				}
				if (agentes[i].mX < 0 )
					agentes[i].mX = 0;
				if (agentes[i].mX > 9 )
					agentes[i].mX = 9;
				if (agentes[i].mY < 0 )
					agentes[i].mY = 0;
				if (agentes[i].mY > 9 )
					agentes[i].mY = 9;
				if (agentes[i].mX == 0 || agentes[i].mX == 9 || agentes[i].mY == 0 || agentes[i].mY == 9){
					if (agentes[i].mD == 0)
						agentes[i].mD = 1;
					else
						agentes[i].mD = 0;
				}
			}
			else
				agentes[i].mK = rand()%5;
		}
		else{
			agentes[i].mK--;
		}
	}
}

inline static void BUILD_MESSAGE_AGENTS(std::vector<rva::CClient> &agents,std::map<int,rva::CClient>& cli){

    std::string mensaje;
    boost::property_tree::ptree pt,jugadores;
    pt.put("tipo_mensaje", 2);
    //jugadores.put("tipo_mensaje", 2);
    std::vector<rva::CClient>::iterator iter = agents.begin();
    while (iter != agents.end()) {
        boost::property_tree::ptree jugadorI;
        jugadorI.put("id",iter->mID);
        
        jugadorI.put("activo",iter->mActivo);
        jugadorI.put("x",iter->mX);
        jugadorI.put("y",iter->mY);
        jugadorI.put("z",iter->mZ);
        jugadorI.put("robot",iter->mTypeClient);
        jugadores.push_back(std::make_pair("", jugadorI));
        iter++;
    }
    
    
    std::map<int,rva::CClient>::iterator iter2 = cli.begin();
    while(iter2 != cli.end())
    {
        boost::property_tree::ptree jugadorI;
        jugadorI.put("id",iter2->second.mID);
        jugadorI.put("x",iter2->second.mX);
        jugadorI.put("y",iter2->second.mY);
        jugadorI.put("z",iter2->second.mZ);
        jugadorI.put("robot",iter2->second.mTypeClient);
        jugadores.push_back(std::make_pair("",jugadorI));
        iter2++;
    }
    pt.add_child("jugadores",jugadores);
    std::stringstream ss;
    write_json(ss, pt,false);

    rva::CGame::getInstance().lock();
    rva::CGame::getInstance().mMessage = ss.str();
    rva::CGame::getInstance().unlock();
}
#endif /* defined(__SocketRVA__CClient__) */