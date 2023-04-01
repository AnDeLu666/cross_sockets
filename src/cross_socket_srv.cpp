#include "cross_socket_srv.h"

//temporary
#include <cstring>

namespace cross_socket
{
    CrossSocketSrv::CrossSocketSrv(int socket_type, unsigned int port)
        : CrossSocket(socket_type), _port(port)
    {
        if(socket_type == TCP)
        {

        }
        else if(socket_type == UDP)
        {

        }
        else
        {
            _sock_error = SocketError::SOCK_UNKNOWN_TYPE;
        }
    }

    void CrossSocketSrv::AcceptConnection()
    {
        int conn_socket;

        int addrlen = sizeof(_address);

        if ((conn_socket = accept(_socket, (struct sockaddr *)&_address, (socklen_t *)&addrlen)) < 0)
        {
            perror("Socket accepting failed");
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
        while (TCP_Recv(_connections[index]) > 0)
        {
            PRINT_DBG("received from client : %s %d\n", _connections[index]->GetBufferFrom().data, _connections[index]->_conn_socket);
            _connections[index]->SetBufferTo((char*)"\0", 1);

            TCP_Send(_connections[index]);
            
            if(std::strcmp(_connections[index]->GetBufferFrom().data, "exit1") == 0)
            {
                PRINT_DBG("break\n");
                break;
            }
        }

        _continue_work = false;

    }

    void CrossSocketSrv::Start()
    {
        if(cross_socket::Server_Init(_socket, _port, _opt, _address) == SocketError::EMPTY)
        {
            // start accepting connections
            _accept_thread = std::thread(&CrossSocketSrv::AcceptConnection, this);
            _accept_thread.detach();
        }
        PRINT_DBG("port %d \n", _address.sin_port);
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
        PRINT_DBG("srv_destr \n");
        this->Stop();
    }

} // end namespace cross_socket