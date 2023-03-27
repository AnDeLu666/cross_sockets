#include "cross_socket_srv.h"

namespace cross_socket
{
    CrossSocketSrv::CrossSocketSrv(unsigned int port)
        : CrossSocket(port)
    {
    }

    void CrossSocketSrv::AcceptConnection()
    {
        _status = Status::SOCK_ACCEPT_CONNECTION;

        int conn_socket;

        if ((conn_socket = accept(_socket, (struct sockaddr *)&_address, (socklen_t *)&_addrlen)) < 0)
        {
            perror("Socket accepting failed");
            _status = Status::SOCK_ACCEPT_ERROR;
        }
        else
        {
            std::string index = std::to_string(conn_socket);
            _connections[index] = std::make_shared<Connection>(conn_socket);
            _connections[index]->_thread = std::thread(&CrossSocketSrv::ConnectionHandler, this, index);
            _connections[index]->_thread.detach();

            AcceptConnection();
        }
    }

    void CrossSocketSrv::ConnectionHandler(std::string index)
    {
        while (_connections[index]->Recv() > 0)
        {
            printf("received from client : %s %d\n", _connections[index]->GetBuffer(), _connections[index]->_conn_socket);
            _connections[index]->Send("\0");
            
            if(std::strcmp(_connections[index]->GetBuffer(), "exit1") == 0)
            {
                printf("break\n");
                break;
            }
        }

        _status = cross_socket::STOP;
    }

    void CrossSocketSrv::Start()
    {
        // Forcefully attaching socket to _port
        if (setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                       &_opt, sizeof(_opt)))
        {
            perror("Setsockopt failure");
            _status = Status::SOCK_SETOPT_ERROR;
        }

        _address.sin_addr.s_addr = INADDR_ANY;
        _address.sin_port = htobe16(_port);

        // Forcefully attaching socket to above defined port S_PORT

        if (bind(_socket, (struct sockaddr *)&_address, sizeof(_address)) < 0)
        {
            perror("Socket binding failed");
            _status = Status::SOCK_BIND_ERROR;
        }

        if (listen(_socket, 3) < 0)
        {
            perror("Socket listening failed");
            _status = Status::SOCK_LISTEN_ERROR;
        }
        else
        {
            // start accepting connections
            _accept_thread = std::thread(&CrossSocketSrv::AcceptConnection, this);
            _accept_thread.detach();
        }
    }

    void CrossSocketSrv::Stop()
    {
        if (_accept_thread.joinable())
        {
            _accept_thread.join();
        }
    }

    CrossSocketSrv::~CrossSocketSrv()
    {
        printf("srv_destr \n");
        this->Stop();
    }

} // end namespace cross_socket