#pragma once

#include "cross_socket_clnt.h"


namespace cross_socket
{
    class CrossSocketClntTCP : public CrossSocketClnt
    {
        public:
            CrossSocketClntTCP();

            void MainHandler(std::string index); 

            ~CrossSocketClntTCP();
    };

}//end namespace cross_socket
