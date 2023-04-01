#include "cross_socket.h"

namespace cross_socket
{
    CrossSocket::CrossSocket(int socket_type)
    : _socket_type(socket_type)
    {   
        // Creating socket file descriptor
        if((_socket = socket(AF_INET,_socket_type, 0)) < 0)
        {
            perror("Socket init failure");
            _sock_error = SocketError::SOCK_INIT_ERROR;
        }

        _address.sin_family = AF_INET; //IPV4
        _address.sin_addr.s_addr = INADDR_ANY;
    }

    SocketError CrossSocket::GetSockError()
    {
        return _sock_error;
    }
    
} //end namespace cross_socket