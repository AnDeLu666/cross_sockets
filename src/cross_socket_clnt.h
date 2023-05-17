#pragma once

#include "cross_socket.h"
#include "cross_socket_conn_wrapper.h"


namespace cross_socket
{

    class CrossSocketClnt : public CrossSocket
    {   
        private:
            bool ConnectToSocket(Socket socket, struct sockaddr_in& address);

            void SendHandler(std::string index); 

        public:
            CrossSocketClnt(int socket_type);

            std::string Connect(std::string ip_addr_str, uint16_t port);

            void Disconnect(std::string conn_key);

            ~CrossSocketClnt();

    };

}//end namespace cross_socket
