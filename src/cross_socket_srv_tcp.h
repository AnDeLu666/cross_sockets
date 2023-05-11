#pragma once

#include "cross_socket_srv.h"

namespace cross_socket
{

class CrossSocketSrvTCP: public CrossSocketSrv, public CrossSocket
{
    private:
        void AcceptHandler();
        void Start_();
        
        void ReceiveHandler(std::string conn_key);
        void SendHandler(std::string conn_key, Buffer* recv_buff);

    public:
        CrossSocketSrvTCP(uint16_t port);

        void DisconnectClient(std::string conn_key);
        
        ~CrossSocketSrvTCP();

};

}//end namespace cross_socket
