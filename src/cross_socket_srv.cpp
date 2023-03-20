#include "cross_socket_srv.h"

namespace cross_socket
{
    CrossSocketSrv::CrossSocketSrv(unsigned int port)
    : CrossSocket(port)
    {}

    void CrossSocketSrv::AcceptConnection()
    {
        printf("start accept");
        if((_conn_s = accept(_socket, (struct sockaddr*)&_address, (socklen_t*)&_addrlen)) < 0)
        {
            perror("Socket accepting failed");
            _status = Status::SOCK_ACCEPT_ERROR;
        }
        else
        {
            _status = Status::ACCEPTED;
        }
        printf("end start accept");
    }

    void CrossSocketSrv::Start()
    {
        // Forcefully attaching socket to above defined port S_PORT
        if(setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, 
                    &_opt, sizeof(_opt)))
        {
            perror("Setsockopt failure");
            _status = Status::SOCK_SETOPT_ERROR;
        }

        _address.sin_addr.s_addr = INADDR_ANY;
        _address.sin_port = htobe16(_port);

        // Forcefully attaching socket to above defined port S_PORT

        if(bind(_socket, (struct sockaddr*) &_address, sizeof(_address)) < 0)
        {
            perror("Socket binding failed");
            _status = Status::SOCK_BIND_ERROR;
        }

        if(listen(_socket, 3) < 0)
        {
            perror("Socket listening failed");
            _status = Status::SOCK_LISTEN_ERROR;
        }
        else
        {
            Status::SOCK_LISTENING;
        }

        //start accepting connections
        printf("make accept");
        _accept_thread = std::thread(&CrossSocketSrv::AcceptConnection, this);
        _accept_thread.detach();
        
    }

    void CrossSocketSrv::Stop()
    {
        if(_accept_thread.joinable())
        {
            _accept_thread.join(); 
        }
    }

    CrossSocketSrv::~CrossSocketSrv()
    {
        this->Stop();

        this->CloseSocket();
        
        delete[] _buffer;
    }

} //end namespace cross_socket