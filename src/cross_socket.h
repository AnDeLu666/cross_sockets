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

        const int _opt = 1;

        Socket InitNewSocket();

    public:
        ConnectionsMap _connections;  

        CrossSocket(int socket_type, bool init_socket);
        
        SocketError GetSockError();

        cross_socket::Buffer& Recv(Socket recv_sock, sockaddr_in* address);
        cross_socket::Buffer& Recv(Socket recv_sock, sockaddr_in* address, bool buff_size_first);

        int Send(Socket send_sock, Buffer* buff, sockaddr_in* address);
        int Send(Socket send_sock, Buffer* buff, sockaddr_in* address, bool buff_size_first);

        ~CrossSocket();
    };

}//end namespace cross_socket
