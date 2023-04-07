#pragma once

#include "cross_socket.h"

namespace cross_socket
{

class CrossSocketSrv
{
    protected:
        unsigned int _port;

    public:
        CrossSocketSrv(unsigned int port);

        virtual void ConnectionHandler(std::string index) = 0;
        
        virtual void Start() = 0;
        virtual void Stop() = 0;

        virtual ~CrossSocketSrv() = default;
};

}//end namespace cross_socket
