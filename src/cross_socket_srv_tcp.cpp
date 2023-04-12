#include "cross_socket_srv_tcp.h"

//temporary
#include <cstring>

namespace cross_socket
{
    CrossSocketSrvTCP::CrossSocketSrvTCP(uint16_t port)
        : CrossSocketSrv(port), CrossSocket(TCP)
    {}

    void CrossSocketSrvTCP::AcceptHandler()
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
            _connections[index]->_thread = std::thread(&CrossSocketSrvTCP::MainHandler, this, index);
            _connections[index]->_thread.detach();

            AcceptHandler();
        }
    }

    void CrossSocketSrvTCP::MainHandler(std::string index) //separate thread for each client
    {
        while (_status != SrvStatuses::STOP)
        {
            auto recv_buff = Recv(_connections[index]->_conn_socket, _address);
            if (recv_buff.size <= 0)
            {
                //when make asynchronous request add sleep
            }
            else
            {
                PRINT_DBG("received data from client : %s socket: %d\n", recv_buff.data, _connections[index]->_conn_socket);

                struct cross_socket::Buffer send_buff = {(char*)"\0", 1};

                Send(_connections[index]->_conn_socket, send_buff, _address);
            }
        }
    }

    void CrossSocketSrvTCP::Start_()
    {
        if(cross_socket::Server_InitTCP(_socket, _port, _opt, _address) == SocketError::EMPTY)
        {
            // start accepting connections
            _accept_thread = std::thread(&CrossSocketSrvTCP::AcceptHandler, this);
            _accept_thread.detach();
        }
    }

    void CrossSocketSrvTCP::Stop()
    {
        //delete all connections
    }

    CrossSocketSrvTCP::~CrossSocketSrvTCP()
    {
        PRINT_DBG("TCPsrv_destr \n");
        this->Stop();
    }

} // end namespace cross_socket