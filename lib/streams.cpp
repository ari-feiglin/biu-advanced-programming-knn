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

    TCPSocket TCPSocket::accept_connection(int timeout) {
        struct sockaddr_in client;
        unsigned int addr_len = sizeof(client);
        int client_sock = -1;

        if (timeout > 0) {
            if (fcntl(this->fd, F_SETFL, O_NONBLOCK) == -1) {
                throw std::ios_base::failure("error encountered on setting socket to nonblocking, errno: " +
                        std::to_string(errno));
            }

            fd_set fds;
            struct timeval s_timeout = {0};

            s_timeout.tv_sec = timeout;

            FD_ZERO(&fds);
            FD_SET(this->fd, &fds);

            int num_ready_fds = select(this->fd + 1, &fds, NULL, NULL, &s_timeout);
            if (num_ready_fds == 0) return TCPSocket(-1);    // No available connections.
            else if (num_ready_fds == -1) throw std::ios_base::failure("error encountered while selecting available fds, errno: " +
                    std::to_string(errno));
        }

        client_sock = accept(this->fd, (struct sockaddr *) &client,  &addr_len);
        if (client_sock < 0) {
            throw std::ios_base::failure("error encountered while attempting to accept an incoming connection, errno: " +
                    std::to_string(errno));
        }

        return TCPSocket(client_sock);
    }

     char* TCPSocket::receive(size_t& size, bool force_size) {
        if (size == 0) return nullptr;

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
                bytes_read = recv(this->fd, data + i, size - i, 0);

                if (bytes_read < 0) {
                    delete[] data;
                    throw std::ios_base::failure("error encountered while receiving from socket, errno: " +
                            std::to_string(errno));

                } else if (bytes_read == 0) {
                    delete[] data;
                    throw std::ios_base::failure("socket closed before forced reception of data");
                }

                i += bytes_read;
            }
        }

        return data;
    }

    void TCPSocket::send(const void* data, size_t size) {
        int bytes_sent;
        int i = 0;

        while (i < size) {
            bytes_sent = ::send(this->fd, (char*)data + i, size - i, 0);
            if (bytes_sent < 0) {
                throw std::ios_base::failure("error encountered while sending to socket, errno: " +
                        std::to_string(errno));
            }
            i += bytes_sent;
        }
    }

    void TCPSocket::close() {
        if (::close(this->fd) < 0) {
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

    TCPSocket::TCPSocket(const char*  ip, int port, const char*  dest_ip, int dest_port) : TCPSocket(ip, port) {
            connect_to(dest_ip, dest_port);
    }
    // end of client
    
    Address TCPSocket::get_address() {
        struct sockaddr_in addr = {0};
        socklen_t len = sizeof(addr);

        if (getsockname(this->fd, (struct sockaddr*)&addr, &len) < 0) {
            throw std::ios_base::failure("error encountered while getting socket name, errno: " +
                    std::to_string(errno));
        }

        Address a;
        a.ip = std::string(inet_ntoa(addr.sin_addr));
        a.port = addr.sin_port;

        return a;
    }
}

