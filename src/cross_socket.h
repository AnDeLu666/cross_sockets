#pragma once

#include <cstdio>
#include <cstring>
#include <thread>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#endif


namespace cross_socket
{

enum Status
{
    EMPTY,
    ACCEPTED,
    CONNECTION_ERROR,
    CONNECTED,
    INVALID_IP_ERROR,
    SOCK_ACCEPT_ERROR,
    SOCK_BIND_ERROR,
    SOCK_INIT_ERROR,
    SOCK_LISTENING,
    SOCK_LISTEN_ERROR,
    SOCK_SETOPT_ERROR,
};

//type of initialization of a SOCKET
enum ConnType
{
    CLIENT,
    SERVER   
};


class CrossSocket
{
protected:
    int _conn_s = -20, _socket, _valread;
    
    Status _status = EMPTY;

    unsigned int _buff_size{1024}, _port{8666};

    struct sockaddr_in _address;

    const int _opt = 1;

    int _addrlen = sizeof(_address);

    char *_buffer;

public:
    CrossSocket(unsigned int port);
    
    int GetConn_s();
    Status GetStatus();
    
    void Send(const char * message);
    void Recv();
    
    void CloseSocket();

    virtual ~CrossSocket();
};

}//end namespace cross_socket
