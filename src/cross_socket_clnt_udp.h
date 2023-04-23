#pragma once

#include "cross_socket_clnt.h"

namespace cross_socket
{
    class CrossSocketClntUDP : public CrossSocketClnt
    {
        public:
            CrossSocketClntUDP();

            void MainHandler(std::string index); 

            ~CrossSocketClntUDP();
    };

}//end namespace cross_socket
