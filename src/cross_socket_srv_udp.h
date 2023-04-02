#pragma once

#include "cross_socket_srv.h"

namespace cross_socket
{

class CrossSocketSrvUDP: public CrossSocketSrv, public CrossSocket
{
    private:
        struct sockaddr_in _srv_addr;

        void AcceptConnection()
        {};
    
    public:
        CrossSocketSrvUDP(unsigned int port);
        
        void ConnectionHandler(std::string index);

        void Start();
};

}//end namespace cross_socket
