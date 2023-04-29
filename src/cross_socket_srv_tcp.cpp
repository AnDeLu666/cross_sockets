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
        Socket conn_socket = -10; 

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

            if(!SetSockoptTCPKeepAlive(conn_socket, 1))
            {
                perror("SOCK_SETOPT_ERROR");
                _sock_error = SocketError::SOCK_SETOPT_ERROR;
            }

            SetWIN_TCPNonBlockingTCPSocket(conn_socket);//under linux it sets in recv send

            _cw.AddNewConnection(conn_key, conn_socket);

            if(_cw.Find(conn_key))
            {
                //_cw.Set_address_ptr(conn_key, server_address_ptr);
                _cw.Set_status(conn_key, ConnStatuses::CONNECTED);
                std::thread(&CrossSocketSrvTCP::MainHandler, this, conn_key).detach();
            }

            AcceptHandler();
        }
    }

    void CrossSocketSrvTCP::MainHandler(std::string conn_key) //separate thread for each client
    {   
        const short sleep_time_limit = 10;
        short sleep_time = 1; //milliseconds it increases if not receive anything up to 

        while (_cw.Get_status(conn_key) == ConnStatuses::CONNECTED)
        {
            auto recv_buff = Recv(_cw.Get_conn_socket(conn_key), _address);

            if (GetSockoptError(_cw.Get_conn_socket(conn_key)) != SocketError::NO_ERRORS)
            {
                _cw.Set_status(conn_key, ConnStatuses::DISCONNECT);
            }
            else if(recv_buff->real_bytes > 0)
            {
                sleep_time = 1;

                auto send_buff = _main_handler_ptr(&_cw, conn_key, *recv_buff);

                if(Send(_cw.Get_conn_socket(conn_key), *send_buff, _address) <= 0)
                {
                    _cw.Set_status(conn_key, ConnStatuses::DISCONNECT);
                }
            }
            
            delete recv_buff;

            std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));

            sleep_time = (sleep_time < sleep_time_limit) ? sleep_time + 1 : sleep_time_limit;
        }

        //starting a new thread to destroy connection
        std::thread(&CrossSocketSrvTCP::DisconnectClient, this, conn_key).detach();
    }

    void CrossSocketSrvTCP::DisconnectClient(std::string conn_key)
    {
        PRINT_DBG("------------>>>>>>Client going to be disconnected by separate thread \n");
        _cw.DeleteConnection(conn_key);
        PRINT_DBG("------------>>>>>>Client were disconnected \n");
    }

    void CrossSocketSrvTCP::Start_()
    {
        if(cross_socket::Server_InitTCP(_socket, _port, _address) == SocketError::NO_ERRORS)
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