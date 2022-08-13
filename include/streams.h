#pragma once

#include <sys/types.h>
#include <sys/socket.h>

namespace streams {
    /**
     * Interface representing a low level stream (file, socket, etc).
     */
    class Stream {
        /** NOTE: if there is an error when calling the function (recv/send returns -1, etc) these should throw exceptions! **/

        /**
         * Receive an arbitrary amount of information from the Stream.
         * @param size              The (max) size of the data to receive.
         * @param force_size        Ensure that the size received is in fact size.
         *                          This means that the Stream may read multiple times from the file/socket.
         * @return                  A pointer to the data read.
         * @note                    If force_size is false, then change size to the number of bytes actually sent.
         */
        virtual void* receive(size_t& size, bool force_size=true) =0;

        /**
         * Overload the receive function for rvalues.
         * @param size      The size of the data to receive.
         * @return          The data received.
         */
        virtual void* receive(size_t&& size) {
            size_t s = size;
            return this->receive(s);
        }

        /**
         * Receive a type from the Stream (eg. T may/should be a primitive like int or char).
         * @return      What was read from the stream.
         * @note        A call to this overloaded receive method should have the same behavior as:
         *              T* p = stream.receive(sizeof(T)); // It MUST read sizeof(T) bytes, and hence let force_size be.
         *              T obj = *p;
         *              delete[] p; // The void* returned from receive should be allocated.
         */
        /** Note that template functions must be implemented in a header file (or a .tpp file and included in the header) **/
        template <typename T>
        virtual T receive() =0;

        /**
         * Send data through the stream.
         * @param data      The data to send.
         * @param size      The size of the data to send.
         * @note            This should ensure that the correct amount of bytes were sent.
         */
        virtual void send(void* data, size_t size) =0;
    };

    class TCPSocket : public Stream {
        int fd;

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

            /**
             * Wrapper around C's listen function (literally just call listen(this->fd, buffer))
             */
            void listening(int buffer);

            /**
             * Accept a connection request.
             * @return      The socket which connected.
             */
            TCPSocket accept_request();

            /**
             * Connect to a remote socket.
             * @param ip        The ip to connect to.
             * @param port      The port to connect to.
             */
            void connect_to(const char*  ip, int port);

            void* receive(size_t& size, bool force_size=true);

            template <typename T>
            T receive();

            void send(void* data, size_t size);
    };



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
}
