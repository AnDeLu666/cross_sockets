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

    char * CrossSocket::GetBuffer()
    {
        return _buffer;
    }

    int CrossSocket::GetConn_s()
    {
        return _conn_s;
    }

    Status CrossSocket::GetStatus()
    {
        return _status;
    }

    void CrossSocket::Send(const char * data) //todo check what faster strlen or size
    {
        _data_size = std::strlen(data);
        //send data size
        send(_conn_s, (char*)&_data_size, sizeof(u_int32_t), 0);
        //send data
        send(_conn_s, data, std::strlen(data), 0);
    }

    void CrossSocket::Recv()
    {
        //receive data size
        read(_conn_s, (char*)&_data_size, sizeof(u_int32_t));
        //recieve data
        _recieved_bytes = read(_conn_s, _buffer, _data_size);
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