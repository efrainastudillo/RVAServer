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

int main(int argc, const char * argv[])
{
    rva::CServer server(7777);
    server.connect();
    server.start();
    
    
    // main process
    while (true) {
        LOG("Estoy en el main")
        sleep(6);
    }
    
    server.joinThread();
    
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