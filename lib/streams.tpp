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



    // server
    TCPSocket::TCPSocket(const char* ip, int port=0) {
        this->fd = socket(AF_INET, SOCK_STREAM, 0);
        if (this->fd < 0) {
            perror("error creating socket");
            return;
        }
        struct sockaddr_in hint;
        memset(&hint, 0, sizeof(hint));
        hint.sin_family = AF_INET;
        hint.sin_port = htons(port);
        inet_pton(AF_INET, ip, &hint.sin_addr);
        
        if (bind(this->fd, (struct sockaddr *) &hint, sizeof(hint)) < 0) {
            perror("error binding socket");
            return;
        }
    }
    
    void TCPSocket::listening(int buffer) {
        if(listen(this->fd, buffer) < 0) {
            perror("error listening to a socket");
            return;
        }
    }

    TCPSocket TCPSocket::accept_request() {
        struct sockaddr_in client;
        unsigned int addr_len = sizeof(client);
        int client_sock = accept(this->fd,  (struct sockaddr *) &client,  &addr_len);
        if (client_sock < 0) {
            perror("error accepting client");
            return;
        }
        // closing listening socket (? [not sure if im supposed to])
        close(this->fd);
        
        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client.sin_addr, ip, INET_ADDRSTRLEN); // gets the IPV4 address of the socket
        int port = client.sin_port; // the port number of the socket
        return TCPSocket(ip, port);
    }
    // end of server


    


    // client
    void TCPSocket::connect_to(const char*  ip, int port) {
        struct sockaddr_in dest;
        memset(&dest, 0, sizeof(dest));
        dest.sin_family = AF_INET;
        dest.sin_port = htons(port);
        dest.sin_addr.s_addr = inet_addr(ip);
        
    
        if(connect(this->fd, (struct sockaddr *) &dest, sizeof(dest)) < 0) {
            perror("error connecting to server");
            return;
        }
    }

    TCPSocket::TCPSocket(const char*  ip, int port, const char*  dest_ip, int dest_port) {
            this->fd = TCPSocket(ip, port).fd;
            connect_to(dest_ip, dest_port);
    }
    // end of client
    
    
    
    void* receive(size_t& size, bool force_size=true);

    template <typename T>
    T receive();

    void send(void* data, size_t size);








    UDPSocket::UDPSocket(const char*  ip, int port) {}

    void UDPSocket::set_other(const char*  ip, int port) {}

}