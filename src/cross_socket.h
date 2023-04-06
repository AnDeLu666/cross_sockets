#pragma once

#include "cross_socket_common.h"
#include "cross_socket_conn.h"

namespace cross_socket
{
    class CrossSocket
    {
    protected:
        int _socket_type = TCP;

        Socket _socket = -20;
        
        SocketError _sock_error = EMPTY;

        struct sockaddr_in _address;

        const int _opt = 1;

    public:
        bool _continue_work = true;

        ConnectionsMap _connections;  

        CrossSocket(int socket_type);
        
        SocketError GetSockError();

        int Recv(std::string conn_indx, struct sockaddr_in& address);
        int Send(std::string conn_indx, struct sockaddr_in& address);

        virtual ~CrossSocket() = default;
    };

}//end namespace cross_socket
