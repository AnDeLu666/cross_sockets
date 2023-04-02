#pragma once

#include "cross_socket_clnt.h"
#include <iostream>

namespace cross_socket
{
    class CrossSocketClntTCP : public CrossSocketClnt, public CrossSocket
    {
        public:
            CrossSocketClntTCP();

            void ConnectionHandler(std::string index); 

            void Connect(unsigned int port);
            void Disconnect();

            ~CrossSocketClntTCP();
    };

}//end namespace cross_socket
