#pragma once

#include "cross_socket.h"

namespace cross_socket
{

class CrossSocketSrv
{
    protected:
        std::thread _accept_thread;

        unsigned int _port;

        virtual void AcceptConnection() = 0;

    public:
        CrossSocketSrv(unsigned int port);

        virtual void ConnectionHandler(std::string index) = 0;
        
        virtual void Start() = 0;
        void Stop();

        ~CrossSocketSrv();
};

}//end namespace cross_socket
