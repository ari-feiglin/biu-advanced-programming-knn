#include "streams.h"
#include <iostream>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

namespace streams {
    void* receive(size_t& size, bool force_size=true) {}

    template <typename T>
        T receive() {}

    void send(void* data, size_t size) {}




    TCPSocket::TCPSocket(const char* ip, int port=0) {
        this->fd = socket(AF_INET, SOCK_STREAM, 0);
        if (this->fd < 0) {
            perror("error creating socket");
        }
        struct sockaddr_in hint;
        memset(&hint, 0, sizeof(hint));
        hint.sin_family = AF_INET;
        hint.sin_port = htons(port);
        inet_pton(AF_INET, ip, &hint.sin_addr);
        
        if (bind(this->fd, (struct sockaddr *) &hint, sizeof(hint)) < 0) {
            perror("error binding socket");
        }
    }
    
    void TCPSocket::listening(int buffer) {
        if(listen(this->fd, buffer) < 0) {
            perror("error listening to a socket");
        }
    }
    TCPSocket::TCPSocket(const char*  ip, int port, const char*  dest_ip, int dest_port) {}

    

    //TCPSocket::TCPSocket accept() {}
    
    void TCPSocket::connect(const char*  ip, int port) {}




    UDPSocket::UDPSocket(const char*  ip, int port) {}

    void UDPSocket::set_other(const char*  ip, int port) {}

}