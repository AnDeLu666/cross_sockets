#include "cross_socket.h"

namespace cross_socket
{

    CrossSocket::CrossSocket(unsigned int port)
    : _port(port)
    {
        _buffer = new char[_buff_size]{};
        
        // Creating socket file descriptor
        if((_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {
            perror("Socket init failure");
            _status = Status::SOCK_INIT_ERROR;
        }

        _address.sin_family = AF_INET;
    }

    int CrossSocket::GetConn_s()
    {
        return _conn_s;
    }

    Status CrossSocket::GetStatus()
    {
        return _status;
    }

    void CrossSocket::Send(const char * message)
    {
        send(_conn_s, message, std::strlen(message), 0);
        printf("message has been send \n");
    }

    void CrossSocket::Recv()
    {
        _valread = read(_conn_s, _buffer, _buff_size);
        printf("%s \n", _buffer);
    }

    void CrossSocket::CloseSocket()
    {
    #ifdef _WIN32
        closesocket(_conn_s);
        WSACleanup();
    #else    
        close(_conn_s);  // close connection socket 
        shutdown(_conn_s, SHUT_RDWR);  // close listen socket ???
    #endif
    }

    CrossSocket::~CrossSocket()
    {}
    
} //end namespace cross_socket