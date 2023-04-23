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
            uint16_t port = htons(_address.sin_port);
            std::string conn_key = inet_ntoa((&_address)->sin_addr);
            conn_key += ":" + std::to_string(port);

            _cw.AddNewConnection(conn_key, conn_socket);

            if(_cw.Find(conn_key))
            {
                //_cw.Set_address_ptr(conn_key, server_address_ptr);
                _cw.Set_thread_ptr(conn_key, std::make_shared<std::thread>(&CrossSocketSrvTCP::MainHandler, this, conn_key));
                _cw.Set_status(conn_key, ConnStatuses::CONNECTED);
            }

            AcceptHandler();
        }
    }

    void CrossSocketSrvTCP::MainHandler(std::string conn_key) //separate thread for each client
    {
        cross_socket::Buffer * send_buff;
        
        while (_cw.Get_status(conn_key) == ConnStatuses::CONNECTED)
        {
            auto recv_buff = Recv(_cw.Get_conn_socket(conn_key), &_address);
            if (recv_buff.s_r_err == RECV_SEND_ERR)
            {
                break;
            }
            else if(recv_buff.real_bytes > 0)
            {
                send_buff = _main_handler_ptr(&_cw, conn_key, recv_buff);

                if(Send(_cw.Get_conn_socket(conn_key), send_buff, &_address) == RECV_SEND_ERR)
                {
                    break;
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        //starting a new thread to destroy connection
        std::thread(&CrossSocketSrvTCP::DisconnectClient, this, conn_key).detach();
    }

    void CrossSocketSrvTCP::DisconnectClient(std::string conn_key)
    {
        _cw.DeleteConnection(conn_key);
    }

    void CrossSocketSrvTCP::Start_()
    {
        if(cross_socket::Server_InitTCP(_socket, _port, _opt._sock_opt, _address) == SocketError::EMPTY)
        {
            // start accepting connections
            _accept_thread = std::thread(&CrossSocketSrvTCP::AcceptHandler, this);
            
            _status = SrvStatuses::STARTED;
        }
    }

    CrossSocketSrvTCP::~CrossSocketSrvTCP()
    {
        PRINT_DBG("TCPsrv_destr \n");
        Stop();
    }

} // end namespace cross_socket