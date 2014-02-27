//
//  main.cpp
//  SocketRVA
//
//  Created by Efrain Astudillo on 2/5/14.
//  Copyright (c) 2014 Efrain Astudillo. All rights reserved.
//

#include <iostream>

#include "CServer.h"

#include "boost/property_tree/ptree.hpp"
#include "boost/property_tree/json_parser.hpp"

void handle_function(void *userdata,vrpn_TRACKERCB track){
    std::cout<< "pos[0]: "<<track.pos[0]<<"pos[1]: "<<track.pos[1]<< "pos[2]: "<<track.pos[2]<<"quat[0]: "<<track.quat[0]<<"quat[1]: "<<track.quat[1]<<"quat[2]: "<<track.quat[2]<< "quat[3]: "<<track.quat[3]<<"sensor: "<<track.sensor<<std::endl;
    // sprintf(MESSAGE, "%f",track.pos[0]);
}

int main(int argc, const char * argv[])
{
    /*rva::CServer server(7777);
    server.connect();
    server.start();
    
    
    // main process
    while (true) {
        LOG("Estoy en el main")
        sleep(6);
    }
     */
    vrpn_Connection *connection;
    
    char connectionName[128];
    int  port = 3883;
    
    sprintf(connectionName,"200.126.23.195:%d", port);
    
    connection = vrpn_get_connection_by_name(connectionName);
    
    vrpn_Tracker_Remote *tracker = new vrpn_Tracker_Remote("Camara", connection);
    
    tracker->register_change_handler(NULL, handle_function);
    //tracker->unregister_change_handler(NULL, handle_function);
    
    
    std::vector<rva::CClient> magentes;
    
    magentes.push_back(rva::CClient());
    
    std::string msg;
    
    BUILD_MESSAGE_AGENTS(magentes, msg);
    
    LOG(msg)
    
    
    
    
    
    // recibe los datos del vrpn
    while(true)
    {
        tracker->mainloop();
        connection->mainloop();
        usleep(500000);
    }
    //server.joinThread();
    
    return 0;
}


























class B{
public:
    B(){
        mValue = 89;
    }
    
    int mValue;
};
class A{
public:
    A(){
        std::cout<<"Creating"<<std::endl;
        mId = 10;
        mMsg = "Hello World";
        mB = new B();
    }
    A(A& o){
        std::cout<<"copying"<<std::endl;
        *this = o;
    }
    
    A& operator=(A& o){
        std::cout<<"assignment"<<std::endl;
        this->mId = o.mId;
        this->mMsg = o.mMsg;
        this->mB = o.mB;
        return *this;
    }
    A& operator=(A&& o){
        std::cout<<"moving assigment"<<std::endl;
        this->mId = o.mId;
        this->mMsg = o.mMsg;
        this->mB = o.mB;
        o.mMsg = "";
        o.mId = 0;
        o.mB = nullptr;
        return *this;
    }
    A(A&& o){
        std::cout<<"moving"<<std::endl;
        *this = o;
        o.mId = 0;
        o.mMsg = "";
        o.mB =NULL;
    }
    
    A getB(){
        A bm;
        bm.mMsg = "mama pega duro";
        return bm;
    }
    B *mB;
    std::string mMsg;
    int mId;
};