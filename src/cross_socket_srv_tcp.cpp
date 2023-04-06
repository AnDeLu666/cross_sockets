#include "cross_socket_srv_tcp.h"

//temporary
#include <cstring>

namespace cross_socket
{
    CrossSocketSrvTCP::CrossSocketSrvTCP(unsigned int port)
        : CrossSocketSrv(port), CrossSocket(TCP)
    {}

    void CrossSocketSrvTCP::AcceptConnection()
    {
        Socket conn_socket;

        PRINT_DBG("accepting conn");

        int addrlen = sizeof(_address);

        if ((conn_socket = accept(_socket, (struct sockaddr *)&_address, (Socklen_t *)&addrlen)) < 0)
        {
            perror("Socket accepting failed");
        }
        else
        {
            std::string index = std::to_string(conn_socket);
            _connections[index] = std::make_shared<Connection>(conn_socket);
            _connections[index]->_thread = std::thread(&CrossSocketSrvTCP::ConnectionHandler, this, index);
            _connections[index]->_thread.detach();

            AcceptConnection();
        }
    }

    void CrossSocketSrvTCP::ConnectionHandler(std::string index)
    {
        while (Recv(index, _address) > 0)
        {
            PRINT_DBG("received from client : %s %d\n", _connections[index]->GetBufferFrom().data, _connections[index]->_conn_socket);
            _connections[index]->SetBufferTo((char*)"\0", 1);

            Send(index, _address);
            
            if(std::strcmp(_connections[index]->GetBufferFrom().data, "exit1") == 0)
            {
                PRINT_DBG("break\n");
                break;
            }
        }

        _continue_work = false;

    }

    void CrossSocketSrvTCP::Start()
    {
        if(cross_socket::Server_InitTCP(_socket, _port, _opt, _address) == SocketError::EMPTY)
        {
            // start accepting connections
            _accept_thread = std::thread(&CrossSocketSrvTCP::AcceptConnection, this);
            _accept_thread.detach();
        }
    }

} // end namespace cross_socket