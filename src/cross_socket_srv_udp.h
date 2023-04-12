#pragma once

#include "cross_socket_srv.h"

namespace cross_socket
{
    class CrossSocketSrvUDP: public CrossSocketSrv, public CrossSocket
    {
        private:
            struct sockaddr_in _srv_addr;

            void Start_();
            void AcceptHandler();
        
        public:
            CrossSocketSrvUDP(uint16_t port);

            void MainHandler(std::string index);

            void Stop()
            {};

            ~CrossSocketSrvUDP();
    };

}//end namespace cross_socket
