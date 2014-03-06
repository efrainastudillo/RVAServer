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
        
        // flags to manipulate the logic game
    private:
        bool mGameOver;
        int mTypeClient;
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
        static bool mIniciarJuego;
        static int mCantidadEspias;
        static const int CANTIDAD_ESPIAS_MAX;
    private:
        
        
        static std::shared_ptr<CGame> mInstance;
        static std::once_flag           only_one;
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
    boost::property_tree::ptree ptre;
     
     //ptre.put("id", );
     std::stringstream ss;
     boost::property_tree::write_json(ss, ptre,false);
     ss.str().c_str();
    
    char buffer[1024] = {0};
    std::vector<rva::CClient>::iterator iter = agents.begin();
	mMessage.append("{\"tipo_mensaje\":\"2\",\"jugadores\":\"[");
    while (iter != agents.end()) {
		sprintf(buffer,"{\"id\":\"%d\",\"activo\":\"%d\",\"x\":\"%f\",\"y\":%f\",\"robot\":\"%d\"},",iter->mID,iter->mActivo,iter->mX,iter->mY,iter->mD);
      //  sprintf(buffer, "%d,%d,%f,%f,%f,%d" ,iter->mID,iter->mActivo,iter->mX,iter->mY,iter->mZ,iter->mD);
        mMessage.append(buffer);
        iter++;
        //LOG(buffer)
    }
	std::vector<rva::CClient>::iterator iter = cli.begin();
	while (iter != cli.end()) {
		sprintf(buffer,"{\"id\":\"%d\",\"activo\":\"%d\",\"x\":\"%f\",\"y\":%f\",\"robot\":\"%d\"},",iter->mID,iter->mActivo,iter->mX,iter->mY,iter->mD);
      //  sprintf(buffer, "%d,%d,%f,%f,%f,%d" ,iter->mID,iter->mActivo,iter->mX,iter->mY,iter->mZ,iter->mD);
        mMessage.append(buffer);
        iter++;
        //LOG(buffer)
    }
	mMessage.append("]}");
    // id,activo,x,y,z,robot
    
    rva::CGame::getInstance().lock();
    rva::CGame::getInstance().mMessage = std::move(std::string(buffer));
    rva::CGame::getInstance().unlock();
}
#endif /* defined(__SocketRVA__CClient__) */