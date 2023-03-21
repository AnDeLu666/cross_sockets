#pragma once

#include "cross_socket.h"

namespace cross_socket
{

    class CrossSocketClnt : public CrossSocket
    {
    private:


    public:
        CrossSocketClnt();

        void Connect(unsigned int port);
        void Disconnect();

        ~CrossSocketClnt();

    };

}//end namespace cross_socket
