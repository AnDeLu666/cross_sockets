#pragma once

#include "cross_socket_clnt.h"

namespace cross_socket
{
    class CrossSocketClntUDP : public CrossSocketClnt, public CrossSocket
    {
        private:
            Socket GetNewSocket(){return CrossSocket::InitNewSocket();};

            bool ConnectToSocket(Socket socket, struct sockaddr_in& address){return true;};


        public:
            CrossSocketClntUDP();

            void MainHandler(std::string index); 

            void Connect(std::string ip_addr_str, uint16_t port);
            void Disconnect();

            ~CrossSocketClntUDP();
    };

}//end namespace cross_socket
