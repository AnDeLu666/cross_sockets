#pragma once

#include "cross_socket_srv.h"

namespace cross_socket
{

class CrossSocketSrvUDP: public CrossSocketSrv, public CrossSocket
{
    private:
        struct sockaddr_in _srv_addr;
    
    public:
        CrossSocketSrvUDP(unsigned int port);
        
        void ConnectionHandler(std::string index);

        void Start();
        void Stop()
        {};

};

}//end namespace cross_socket
