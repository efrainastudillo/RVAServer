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

class CClient{
public:
    
    CClient();
    CClient(int sockt);

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
    
    void setupVrpn();
    std::string getMsg();
    std::string getMsgRobots();
    
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
    float mX;
    float mY;
    float mZ;
    int mD;
    int mK;
    int mActivo;
    
    // flags to manipulate the logic game
private:
    bool mGameOver;
    int mRobot;
    
};

}




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

inline static void BUILD_MESSAGE_AGENTS(std::vector<rva::CClient> &agents,std::string & msg){
    boost::property_tree::ptree ptre;
     
     //ptre.put("id", );
     std::stringstream ss;
     boost::property_tree::write_json(ss, ptre,false);
     ss.str().c_str();
    
    char buffer[1024] = {0};
    std::vector<rva::CClient>::iterator iter = agents.begin();
    while (iter != agents.end()) {
        sprintf(buffer, "%d,%d,%f,%f,%f,%d" ,iter->mID,iter->mActivo,iter->mX,iter->mY,iter->mZ,iter->mD);
        //mMessage.append(buffer);
        iter++;
        //LOG(buffer)
    }
    // id,activo,x,y,z,robot
    
    msg = std::move(std::string(buffer));
}
#endif /* defined(__SocketRVA__CClient__) */
