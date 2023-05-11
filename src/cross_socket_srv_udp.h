#pragma once

#include "cross_socket_srv.h"

namespace cross_socket
{
    class CrossSocketSrvUDP: public CrossSocketSrv, public CrossSocket
    {
        private:
            struct sockaddr_in _srv_addr;

            void Start_();
            void AcceptHandler();//now receive handler is implemented inside AcceptHandler
            void SendHandler(std::string conn_key, Buffer* recv_buff);
        
        public:
            CrossSocketSrvUDP(uint16_t port);

            void Stop()
            {};

            ~CrossSocketSrvUDP();
    };

}//end namespace cross_socket
