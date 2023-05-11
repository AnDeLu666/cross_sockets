#pragma once

#include "cross_socket_clnt.h"

namespace cross_socket
{
    class CrossSocketClntUDP : public CrossSocketClnt
    {
        private:

            void SendHandler(std::string conn_key); 

        public:
            CrossSocketClntUDP();

            ~CrossSocketClntUDP();
    };

}//end namespace cross_socket
