#pragma once

#include <cstdio>
#include <cstring> 
#include <thread>
#include <map>
#include <memory>

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
        SOCK_ACCEPT_CONNECTION,
        ACCEPTED_START_HANDLER,
        CONNECTION_ERROR,
        CONNECTED,
        INVALID_IP_ERROR,
        SOCK_ACCEPT_ERROR,
        SOCK_BIND_ERROR,
        SOCK_INIT_ERROR,
        SOCK_LISTENING,
        SOCK_LISTEN_ERROR,
        SOCK_SETOPT_ERROR,
        STOP
    };

    //type of initialization of a SOCKET
    // enum ConnType
    // {
    //     CLIENT,
    //     SERVER   
    // };


    class Connection
    {
    private:
        std::shared_ptr<char[]> _buffer = nullptr;
        
        void CloseSocket();

    public:
        std::thread _thread;
        int _conn_socket;

        Connection(int conn_socket);
        
        int Send(const char *data);
        int Recv();

        char * GetBuffer();

        ~Connection();
    };

    typedef std::map<const char*,  std::shared_ptr<Connection>> ConnectionsMap;

    class CrossSocket
    {
    protected:
        int _socket = -20;
        
        Status _status = EMPTY;

        unsigned int _port{8666};

        struct sockaddr_in _address;

        const int _opt = 1;

        int _addrlen = sizeof(_address);

    public:
        ConnectionsMap _connections;  

        CrossSocket(unsigned int port);
        
        Status GetStatus();

        virtual ~CrossSocket();
    };

}//end namespace cross_socket
