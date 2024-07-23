#include <config.hpp>
#include <commands.hpp>
#include <chars.hpp>
#include <types.hpp>
#include <stdio.h>
#include <string.h>
#include <thread>
#include <utility>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define CLIENT_MAX 8

namespace ClearCore {

EthernetTCPServer::EthernetTCPServer(uint16_t port){
    printf("Entering net config.");
    char rx_buf[MAX_BUFFER];
    connfd = 0;
    sockaddr_in servAddr;
    bzero((char*)&servAddr, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(port);
    serverSd = socket(AF_INET, SOCK_STREAM, 0);
    bindStatus = bind(serverSd, (struct sockaddr*) &servAddr, sizeof(servAddr));
    if (bindStatus < 0) printf("Bind Error");
    listen(serverSd, CLIENT_MAX);
    connfd = accept(serverSd, (struct sockaddr*)NULL, NULL);
}
uint32_t EthernetTCPServer::Send(char * buffer){
    write(connfd, buffer, strlen(buffer));
}

uint16_t EthernetTCPServer::Read(char * buffer, uint32_t length){
    read(connfd, buffer, length);
}

EthernetTCPServer::~EthernetTCPServer(){
    close(connfd);
    close(serverSd);
}

class MotorDriver{
public:
    MotorDriver(){
        steps_executing = false;
    }

    
private:
    bool steps_executing = false;
};
    
}