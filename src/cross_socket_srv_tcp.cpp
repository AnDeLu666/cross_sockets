#include "cross_socket_srv_tcp.h"

//temporary
#include <cstring>

namespace cross_socket
{
    CrossSocketSrvTCP::CrossSocketSrvTCP(uint16_t port)
        : CrossSocketSrv(port), CrossSocket(TCP, true)
    {}

    void CrossSocketSrvTCP::AcceptHandler()
    {
        Socket conn_socket;

        int addrlen = sizeof(_address);

        if ((conn_socket = accept(_socket, (struct sockaddr *)&_address, (Socklen_t *)&addrlen)) < 0)
        {
            perror("Socket accepting failed");
        }
        else
        {
            std::string index = std::to_string(conn_socket);
            _connections[index] = std::make_shared<Connection>(conn_socket);
            _connections[index]->Set_thread_ptr(std::make_shared<std::thread>(std::thread(&CrossSocketSrvTCP::MainHandler, this, index)));
            _connections[index]->Get_thread_ptr()->detach();
            _connections[index]->Set_status(ConnStatuses::CONNECTED);

            AcceptHandler();
        }
    }

    void CrossSocketSrvTCP::MainHandler(std::string index) //separate thread for each client
    {
        cross_socket::Buffer * send_buff;
        auto conn = _connections[index];
        
        while (_status != SrvStatuses::STOP)
        {
            auto recv_buff = Recv(conn->Get_conn_socket(), &_address);
            if (recv_buff.real_bytes <= 0)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
            else
            {

                send_buff = _main_handler_ptr(conn, recv_buff);

                Send(_connections[index]->Get_conn_socket(), send_buff, &_address);
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