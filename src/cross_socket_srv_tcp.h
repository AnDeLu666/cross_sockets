#pragma once

#include "cross_socket_srv.h"

namespace cross_socket
{

class CrossSocketSrvTCP: public CrossSocketSrv, public CrossSocket
{
    private:
        void AcceptConnection();

    public:
        CrossSocketSrvTCP(unsigned int port);

        void ConnectionHandler(std::string index);
        
        void Start();

};

}//end namespace cross_socket
