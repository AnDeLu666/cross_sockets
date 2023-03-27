#pragma once

#include "cross_socket.h"
#include <iostream>

namespace cross_socket
{

    class CrossSocketClnt : public CrossSocket
    {
    private:


    public:
        CrossSocketClnt();

        void ConnectionHandler(std::string index); //, char* data); 

        void Connect(unsigned int port);
        void Disconnect();

        ~CrossSocketClnt();

    };

}//end namespace cross_socket
