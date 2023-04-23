#pragma once

#include "cross_socket_common.h"

namespace cross_socket
{
    class CrossSocket
    {
    protected:
        struct Options
        {
            bool _send_recv_buff_size_first = true;
            short _sock_opt = 1;
        };

        Options _opt;

        int _socket_type = TCP;

        Socket _socket = -20;
        
        SocketError _sock_error = EMPTY;

        Socket InitNewSocket();
        void SetSockOptTimeout(Socket socket, short timeout_in_seconds);

    public:
        CrossSocket(int socket_type, bool init_socket);

        void SetOptions(bool send_recv_buff_size_first);
        
        SocketError GetSockError();

        cross_socket::Buffer& Recv(Socket recv_sock, sockaddr_in* address);
        int Send(Socket send_sock, Buffer* buff, sockaddr_in* address);

        ~CrossSocket();
    };

}//end namespace cross_socket
