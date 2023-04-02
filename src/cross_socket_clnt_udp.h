#pragma once

#include "cross_socket_clnt.h"
#include <iostream>

namespace cross_socket
{
    class CrossSocketClntUDP : public CrossSocketClnt, public CrossSocket
    {
        public:
            CrossSocketClntUDP();

            void ConnectionHandler(std::string index); 

            void Connect(unsigned int port);
            void Disconnect();

            ~CrossSocketClntUDP();
    };

}//end namespace cross_socket