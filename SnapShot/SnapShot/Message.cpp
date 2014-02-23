//
//  Message.cpp
//  SnapShot
//
//  Created by KamCheung Ting on 2/20/14.
//  Copyright (c) 2014 KamCheung Ting. All rights reserved.
//

#include "Message.h"
#include <sstream>
#include <iomanip>
#include <unistd.h>
using namespace std;

const char* Message::ACK_MSG_FORMAT = "{action:ack, id:%09d, ip:%s, port:%05d}";
const char* Message::ACK_MSG_PARSE = "{action:%10[^,], id:%d, ip:%15[^,], port:%d}";
const char* Message::ACTION_HEADER_FORMAT = "action:%04d\r\nlength:%010d\r\n";
const char* Message::ACTION_HEADER_PARSE = "action:%d\r\nlength:%d\r\n";
const char* Message::ACTION_MSG_FORMAT = "{money:%05d, widgets:%05, time:%012d, vector:%s}";
const char* Message::ACTION_MSG_PARSE = "{money:%d, widgets:%d, time:%d}";
const char* Message::ACTION_MARKER_PARSE = "{time:%d}";
const char* Message::ACTION_MARKER = "{time:%012d, vector:%s}";

Message::Message(){
    _action = PURCHASE_ACTION;
    _widgets = 0;
    _money = 0;
    _time = 0;
    _timeVector = std::vector<unsigned>();
}

void parseTimeVector( vector<unsigned>& timeVector, string& input, string delimiter){
    size_t pos = 0;
    int value = 0;
    while((pos = input.find(delimiter)) != string::npos){
        string token = input.substr(0, pos);
        istringstream(token)>>value;
        input.erase(0, pos + delimiter.length());
        timeVector.push_back(value);
    }
}

Message::Message(unsigned action, char* msg){
    std::string delimiter = ", ";
    if(action == PURCHASE_ACTION || action == DELIVERY_ACTION){
        std::string timeString(msg);
        sscanf(msg, ACTION_MSG_PARSE, &_money, &_widgets, &_time);
        timeString = timeString.substr(timeString.find("[") + 1, timeString.find("]") - timeString.find("[") - 1);
        _action = action;
        _timeVector = std::vector<unsigned>();
        parseTimeVector(_timeVector, timeString, delimiter);
    }else{
        std::string timeString(msg);
        sscanf(msg, ACTION_MARKER_PARSE, &_time);
        timeString = timeString.substr(timeString.find("[") + 1, timeString.find("]") - timeString.find("[") - 1);
        _action = action;
        _widgets = 0;
        _money = 0;
        _timeVector = std::vector<unsigned>();
        parseTimeVector(_timeVector, timeString, delimiter);
    }
}

std::string Message::toString(){
    using namespace std;
    if(_action == PURCHASE_ACTION||_action == DELIVERY_ACTION){
        char buff[ACTION_HEADER_SIZE+1];
        stringstream ss;
        ss << "{money:"<<setw(5)<<setfill('0')<<_money<<", ";
        ss << "widgets:"<<setw(5)<<setfill('0')<<_widgets<<", ";
        ss << "time:"<<setw(12)<<setfill('0')<<_time<<", ";
        ss << "vector:[";
        for(int i = 0; i < _timeVector.size(); ++i){
            ss<<_timeVector[i]<<", ";
        }
        ss << "]}";
        string content = ss.str();
        sprintf(buff, ACTION_HEADER_FORMAT, _action, content.size());
        ss.str("");
        ss<<buff<<content;
        return ss.str();
    }else{
        char buff[ACTION_HEADER_SIZE+1];
        stringstream ss;
        ss << "{time:"<<setw(12)<<setfill('0')<<_time<<", ";
        ss << "vector:[";
        for(int i = 0; i < _timeVector.size(); ++i){
            ss<<_timeVector[i]<<", ";
        }
        ss << "]}";
        string content = ss.str();
        sprintf(buff, ACTION_HEADER_FORMAT, _action, content.size());
        ss.str("");
        ss<<buff<<content;
        return ss.str();
    }
};

char* Message::toCharArray(){
    std::string stringFormat = toString();
    size_t size =  stringFormat.size();
    char* result = new char[size + 1];
    strcpy(result, stringFormat.c_str());
    return result;
};


const char* InitMessage::INIT_MESSAGE_FORMAT = "{id:%09d, ip:%s, port:%05d}";
const char* InitMessage::INIT_HEADER_FORMAT = "action:0000\r\nlength:%010d\r\n";
const char* InitMessage::INIT_MESSAGE_PARSE = "action:%d\r\nlength:%d\r\n{id:%d, ip:%15[^,], port:%d}";
char* InitMessage::toCharArray(int pid, const char* ip, int port){
    char content[100];
    char header[100];
    char* result;
    std::stringstream ss;
    unsigned size = 0;
    sprintf(content, INIT_MESSAGE_FORMAT, pid, ip, port);
    size = (unsigned)strlen(content);
    sprintf(header, INIT_HEADER_FORMAT, size);
    ss << header << content;
    result = new char[ss.str().length() + 1];
    strcpy(result, ss.str().c_str());
    std::cout << "send init message => "<<ss.str()<<std::endl;
    return result;
};

