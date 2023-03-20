#pragma once

#include "cross_socket.h"

namespace cross_socket
{

    class CrossSocketClnt : public CrossSocket
    {
    private:


    public:
        CrossSocketClnt(unsigned int port);

        void Connect();
        void Disconnect();

        ~CrossSocketClnt();

    };

}//end namespace cross_socket
