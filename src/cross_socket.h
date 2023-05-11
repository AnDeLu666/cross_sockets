#pragma once

#include "cross_socket_common.h"
#include "cross_socket_conn_wrapper.h"

namespace cross_socket
{
    class CrossSocket
    {
    protected:
        //customize function which deal with data and return send buffer
        std::function<Buffer*(ConnectionsWrapper* cw, std::string conn_key, Buffer*)> _received_data_handler = nullptr;
        
        //if true Send handler will send data to current connection socket address
        //else you have to implement sending in function _received_data_handler
        bool _send_data = true;

        int _socket_type = TCP;

        Socket _socket = -20;
        
        SocketError _sock_error = NO_ERRORS;

        Socket InitNewSocket();
        void SetSockOptTimeout(Socket socket, short recv_send_timeout_sec);

        bool ProtocolHandlerRecv(My_ssize_t recv_bytes, Buffer* buff, byte_t* buffer);
        uint16_t ProtocolHandlerSend(Buffer& buff, data_size_t real_data_size);

    public:
        ConnectionsWrapper _cw;  // connections wrapper  

        CrossSocket(int socket_type, bool init_socket);
        
        SocketError GetSockError();

        void Set_received_data_handler(std::function<Buffer*(cross_socket::ConnectionsWrapper* cw, std::string conn_key, Buffer*)> func_ptr);
        void Set_default_send_data_mode(bool send_data); //true SendHandler will call func Send - false will not

        cross_socket::Buffer* Recv(Socket recv_sock); // use in TCP
        cross_socket::Buffer* Recv(Socket recv_sock, sockaddr_in& address);

        int Send(Socket send_sock, Buffer* buff); // use in TCP
        int Send(Socket send_sock, Buffer* buff, sockaddr_in& address);

        ~CrossSocket();
    };

}//end namespace cross_socket
