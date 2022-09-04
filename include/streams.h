#pragma once

#include <stdexcept>
#include <ios>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <fcntl.h>

namespace streams {
    /**
     * Interface representing a low level stream (file, socket, etc).
     */
    struct Stream {
        /** NOTE: if there is an error when calling the function (recv/send returns -1, etc) these should throw exceptions! **/

        /**
         * Receive an arbitrary amount of information from the Stream.
         * @param size              The (max) size of the data to receive.
         * @param force_size        Ensure that the size received is in fact size.
         *                          This means that the Stream may read multiple times from the file/socket.
         * @return                  A pointer to the data read.
         * @note                    If force_size is false, then change size to the number of bytes actually sent.
         */
        virtual char* receive(size_t& size, bool force_size=true) =0;

        /**
         * Overload the receive function for rvalues.
         * @param size      The size of the data to receive.
         * @return          The data received.
         */
        char* receive(size_t&& size) {
            size_t s = size;
            return this->receive(s);
        }

        /**
         * Receive a type from the Stream (eg. T may/should be a primitive like int or char).
         * @return      What was read from the stream.
         */
        template <typename T>
        T receive() {
            T prim;
            char* data = receive(sizeof(T));
            prim = *(T*)data;
            delete[] data;
            return prim;
        }

        /**
         * Send data through the stream.
         * @param data      The data to send.
         * @param size      The size of the data to send.
         * @note            This should ensure that the correct amount of bytes were sent.
         */
        virtual void send(const void* data, size_t size) =0;

        /**
         * Checks if the Stream is associated with a valid... stream.
         */
        virtual bool is_good() =0;

        /**
         * Closes the stream.
         */
        virtual void close() =0;

        /**
         * The destructor also closes the stream.
         */
        /*virtual ~Stream() =0;*/
    };

    struct Address {
        std::string ip;
        int port;
    };

    class TCPSocket : public Stream {
        int fd;

        /**
         * Constructor for creating TCP sockets out of file descriptors.
         * This should only be called by a method like accept_connection.
         * @param socket_fd     The file descriptor to use.
         */
        TCPSocket(int socket_fd) : fd(socket_fd) { }

        public:
            /**
             * Construct a TCP socket (also binds the socket).
             * @param ip        The ip to bind to.
             * @param port      The port to bind to.
             */
            TCPSocket(const char* ip, int port=0);

            /**
             * Construct a TCP socket, bind, and connect.
             * @param ip            The ip to bind to.
             * @param port          The port to bind to.
             * @param dest_ip       The ip to connect to.
             * @param dest_port     The port to connect to.
             */
            TCPSocket(const char*  ip, int port, const char*  dest_ip, int dest_port);

            TCPSocket(std::string ip, int port=0) : TCPSocket(ip.c_str(), port) { }
            TCPSocket(std::string ip, int port, std::string dest_ip, int dest_port) :
                TCPSocket(ip.c_str(), port, dest_ip.c_str(), dest_port) { }

            TCPSocket(const TCPSocket& other) : fd(other.fd) { }

            /**
             * Wrapper around C's listen function (literally just call listen(this->fd, buffer))
             */
            void listening(int buffer);

            /**
             * Accept a connection request.
             * @param timeout       The timeout for the accepted connection. -1 for blocking (if the socket
             *                      is still blocking).
             * @return              The socket which connected. If the acceptance is timed out, then a TCPSocket
             *                      is returned where a call to .is_good() returns false.
             * Note that if timeout is specified (>=0) then the socket is assumed to be used only
             * for timeout'd connections, and is therefore set to non-blocking.
             */
            TCPSocket accept_connection(int timeout=-1);

            /**
             * Connect to a remote socket.
             * @param ip        The ip to connect to.
             * @param port      The port to connect to.
             */
            void connect_to(const char* ip, int port);
            void connect_to(std::string ip, int port) { this->connect_to(ip.c_str(), port); }

            char* receive(size_t& size, bool force_size=true) override;

            /*template <typename T>
            T receive() override {
                T primitive;
                int bytes_read = 0;
                int i = 0;

                while (i < sizeof(primitive)) {
                    bytes_read = recv(this->fd, &primitive + i, sizeof(primitive) - i);

                    if (bytes_read < 0) {
                        throw std::ios_base::failure("error encountered while receiving from socket, errno: " +
                                std::to_string(errno));

                    } else if (bytes_read == 0) {
                        throw std::ios_base::failure("socket closed before forced reception of data");
                    }

                    i += bytes_read;
                }

                return primitive;
            }*/

            void send(const void* data, size_t size) override;

            /**
             * Gets the address of the socket.
             */
            Address get_address();

            bool is_good() override { return this->fd >= 0; }

            void close() override;
    };

    #ifdef DEF_UDP
    // no need for UDP yet. 
    class UDPSocket : public Stream {
        int fd;
        struct sockaddr_in other;

        public:
            /**
             * Construct a UDP socket.
             * @param ip        The ip to bind to.
             * @param port      The port to bind to.
             */
            UDPSocket(const char*  ip, int port);

            /**
             * Sets the destination and source for subsequent calls to send and receive.
             * @param ip        The remote ip to set as the destination/source ip.
             * @param port      The port to set as the destination/source port.
             */
            void set_other(const char*  ip, int port);

            /** If you want to, maybe also overload Stream's send and receive functions so they can also take as arguments the destination/source address. **/
    };
    #endif
}
