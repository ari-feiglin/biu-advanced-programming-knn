#include "streams.h"
#include <iostream>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

namespace streams {
    // server
    TCPSocket::TCPSocket (int fd) {
        this->fd;
    }

    TCPSocket::TCPSocket(const char* ip, int port) {
        this->fd = socket(AF_INET, SOCK_STREAM, 0);
        if (this->fd < 0) {
            throw std::ios_base::failure("error encountered while initializing socket, errno: " +
                    std::to_string(errno));
        }

        /* Only bind if necessary */
        if (port > 0) {
            struct sockaddr_in hint = {0};

            hint.sin_family = AF_INET;
            hint.sin_port = htons(port);
            if (inet_pton(AF_INET, ip, &hint.sin_addr) <= 0) {
                throw std::ios_base::failure("could not bind to requested IP address");
            }

            int opt = 0;
            /* Make port and address reusable */
            if (setsockopt(this->fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
                throw std::ios_base::failure("error encountered when changing socket options, errno: " +
                        std::to_string(errno));
            }

            if (bind(this->fd, (struct sockaddr *) &hint, sizeof(hint)) < 0) {
                throw std::ios_base::failure("error encountered while binding socket, errno: " +
                        std::to_string(errno));
            }
        }
    }
    
    void TCPSocket::listening(int buffer) {
        if(listen(this->fd, buffer) < 0) {
            throw std::ios_base::failure("error encountered while attempting to listen for connections, errno: " +
                    std::to_string(errno));
        }
    }

    TCPSocket TCPSocket::accept_connection() {
        struct sockaddr_in client;
        unsigned int addr_len = sizeof(client);
        int client_sock = accept(this->fd, (struct sockaddr *) &client,  &addr_len);
        if (client_sock < 0) {
            throw std::ios_base::failure("error encountered while attempting to accept an incoming connection, errno: " +
                    std::to_string(errno));
        }
        
        return TCPSocket(client_sock);
    }

     char* TCPSocket::receive(size_t& size, bool force_size) {
        char* data = new char[size];
        int bytes_read = 0;

        if (!force_size) {
            size = recv(this->fd, data, size, 0);

            if (size < 0) {
                throw std::ios_base::failure("error encountered while receiving from socket, errno: " +
                        std::to_string(errno));
            } else if (size == 0) {       // Remote socket closed connection
                return nullptr;
            }
        } else {
            int i = 0;

            while (i < size) {      // This will continue its loop until the number of bytes received equals the number requested.
                bytes_read = recv(this->fd, data + i, size - i);

                if (bytes_read < 0) {
                    throw std::ios_base::failure("error encountered while receiving from socket, errno: " +
                            std::to_string(errno));

                } else if (bytes_read == 0) {
                    throw std::ios_base::failure("socket closed before forced reception of data");
                }

                i += bytes_read;
            }
        }

        return data;
    }

    void TCPSocket::send(void* data, size_t size) {
        int bytes_sent;
        int i = 0;

        while (i < size) {
            bytes_sent = send(this->fd, data + i, size - i);
            if (bytes_sent < 0) {
                throw std::ios_base::failure("error encountered while sending to socket, errno: " +
                        std::to_string(errno));
            }
            i += bytes_sent;
        }
    }

    void TCPSocket::send(std::string str) {
        this->send(str.c_str(), str.length() + 1);
    }

    void TCPSocket::close() {
        if (close(this->fd) < 0) {
            throw std::ios_base::failure("error encountered while closing socket, errno: " +
                    std::to_string(errno));
        }
    }

    // client
    void TCPSocket::connect_to(const char*  ip, int port) {
        struct sockaddr_in dest = {0};

        dest.sin_family = AF_INET;
        dest.sin_port = htons(port);
        if (inet_pton(AF_INET, ip, &(dest.sin_addr.s_addr)) < 0) {
            throw std::ios_base::failure("could not access requested IP address");
        }

        if(connect(this->fd, (struct sockaddr *) &dest, sizeof(dest)) < 0) {
            throw std::ios_base::failure("error encountered while attempting to connect to remote socket, errno: " +
                    std::to_string(errno));
        }
    }

    TCPSocket::TCPSocket(const char*  ip, int port, const char*  dest_ip, int dest_port) {
            this->fd = TCPSocket(ip, port).fd;
            connect_to(dest_ip, dest_port);
    }
    // end of client
}

