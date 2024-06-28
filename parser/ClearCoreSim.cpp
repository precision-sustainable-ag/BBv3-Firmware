#include <config.hpp>
#include <stdio.h>
#include <string.h>
#include <thread>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define CLIENT_MAX 8

namespace ClearCore {

class EthernetTCPServer {
public:
    EthernetTCPServer(){
        char rx_buf[MAX_BUFFER];
        sockaddr_in servAddr;
        bzero((char*)&servAddr, sizeof(servAddr));
        servAddr.sin_family = AF_INET;
        servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
        servAddr.sin_port = htons(PORT);
        serverSd = socket(AF_INET, SOCK_STREAM, 0);
        bindStatus = bind(serverSd, (struct sockaddr*) &servAddr, sizeof(servAddr));
        listen(serverSd, CLIENT_MAX);
    }
    int write(const char * buffer){
        
    }
private:
    int serverSd, bindStatus;
};

class MotorDriver{
public:
    MotorDriver(){
        steps_executing = false;
    }

    
private:
    bool steps_executing = false;
};
    
}