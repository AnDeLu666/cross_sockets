#pragma once

#include "cross_socket_common.h"

namespace cross_socket
{
    class CrossSocket
    {
    protected:
        int _socket_type = TCP;

        Socket _socket = -20;
        
        SocketError _sock_error = NO_ERRORS;

        Socket InitNewSocket();
        void SetSockOptTimeout(Socket socket, short recv_send_timeout_sec);

    public:
        CrossSocket(int socket_type, bool init_socket);
        
        SocketError GetSockError();

        //we pass and receive data by & because connection which keeps buff and address 
        //could be deleted and if pass by & at least we winish job correctly TODO maybe better use shared ptr
        cross_socket::Buffer* Recv(Socket recv_sock, sockaddr_in& address);
        int Send(Socket send_sock, Buffer& buff, sockaddr_in& address);

        ~CrossSocket();
    };

}//end namespace cross_socket
