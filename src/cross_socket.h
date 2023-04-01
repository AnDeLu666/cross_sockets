#pragma once

#include "cross_socket_common.h"

namespace cross_socket
{
    class CrossSocket
    {
    protected:
        int _socket_type;

        int _socket = -20;
        
        SocketError _sock_error = EMPTY;

        struct sockaddr_in _address;

        const int _opt = 1;

    public:
        bool _continue_work = true;

        ConnectionsMap _connections;  

        CrossSocket(int socket_type);
        
        SocketError GetSockError();

        virtual ~CrossSocket() = default;
    };

}//end namespace cross_socket
