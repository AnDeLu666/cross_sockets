#pragma once

#include "cross_socket.h"

namespace cross_socket
{

    class CrossSocketClnt 
    {
        public:
            virtual void ConnectionHandler(std::string index) = 0; 

            virtual void Connect(std::string ip_addr_str, unsigned int port) = 0;
            virtual void Disconnect() = 0;

            ~CrossSocketClnt();

    };

}//end namespace cross_socket
