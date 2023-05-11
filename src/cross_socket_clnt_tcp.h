#pragma once

#include "cross_socket_clnt.h"


namespace cross_socket
{
    class CrossSocketClntTCP : public CrossSocketClnt
    {
        private:
            void SendHandler(std::string conn_key); 

        public:
            CrossSocketClntTCP();

            ~CrossSocketClntTCP();
    };

}//end namespace cross_socket
