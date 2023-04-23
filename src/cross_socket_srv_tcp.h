#pragma once

#include "cross_socket_srv.h"

namespace cross_socket
{

class CrossSocketSrvTCP: public CrossSocketSrv, public CrossSocket
{
    private:
        void AcceptHandler();
        void Start_();

    public:
        CrossSocketSrvTCP(uint16_t port);

        void MainHandler(std::string index);

        void DisconnectClient(std::string conn_key);
        
        ~CrossSocketSrvTCP();

};

}//end namespace cross_socket
