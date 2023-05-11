#pragma once

#include "cross_socket.h"
#include "cross_socket_conn_wrapper.h"


namespace cross_socket
{

    class CrossSocketClnt : public CrossSocket
    {   
        private:
            bool ConnectToSocket(Socket socket, struct sockaddr_in& address);

            virtual void SendHandler(std::string index) = 0; 

        public:
            CrossSocketClnt(int socket_type, bool init_socket);

            virtual std::string Connect(std::string ip_addr_str, uint16_t port);

            virtual void Disconnect(std::string conn_key);

            ~CrossSocketClnt();

    };

}//end namespace cross_socket
