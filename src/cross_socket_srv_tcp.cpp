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
            auto conn_key = GetIP_PortStringFromAddress(_address);

            //todo check is it really neccessary
            if(!SetSockoptTCPKeepAlive(conn_socket, 1)) //1 turn on 0 turn off
            {
                perror("SOCK_SETOPT_ERROR");
                _sock_error = SocketError::SOCK_SETOPT_ERROR;
            }

            SetWIN_TCPNonBlockingTCPSocket(conn_socket);//under linux it sets in recv send

            PRINT_DBG("here");

            if(!_cw.Find(conn_key))
            {
                _cw.AddNewConnection(conn_key, conn_socket);

                sockaddr_in address{_address};
                _cw.Set_address_ref(conn_key, address);
                _cw.Set_status(conn_key, ConnStatuses::CONNECTED);
                std::thread(&CrossSocketSrvTCP::ReceiveHandler, this, conn_key).detach();
            }

            AcceptHandler();
        }
    }

    void CrossSocketSrvTCP::ReceiveHandler(std::string conn_key) //separate thread for each client
    {   
        const short sleep_time_limit = 100;
        short sleep_time = 1; //milliseconds it increases if not receive anything up to 

        //int timer = 0;

        while (_cw.Get_status(conn_key) == ConnStatuses::CONNECTED)
        {
            auto recv_buff = Recv(_cw.Get_conn_socket(conn_key));

            if (GetSockoptError(_cw.Get_conn_socket(conn_key)) != SocketError::NO_ERRORS)
            {
                _cw.Set_status(conn_key, ConnStatuses::DISCONNECT);
            }
            else if(recv_buff->real_bytes > 0)
            {
                sleep_time = 1;
                if(_received_data_handler != nullptr)
                {
                    SendHandler(conn_key, recv_buff);
                }
            }
            else
            {       
                delete recv_buff;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
            sleep_time = (sleep_time < sleep_time_limit) ? sleep_time + 1 : sleep_time_limit;

            // if(timer == 5000)
            // {
            //     timer = 0;
            //     PRINT_DBG("GET Hungs!!!!Timeout is reached conn : %s\n", conn_key.c_str());
            //     _cw.Set_status(conn_key, ConnStatuses::DISCONNECT);
            //     // Buffer *buff = new cross_socket::Buffer{};
            //     // if(Send(_cw.Get_conn_socket(conn_key), *buff) <= 0)
            //     // {
            //     //     PRINT_DBG("GET Hungs!!!!DISCONNECT \n");
            //     //     _cw.Set_status(conn_key, ConnStatuses::DISCONNECT);
            //     // }
            // }

            // timer++;    
        }

        //starting a new thread to destroy connection
        std::thread(&CrossSocketSrvTCP::DisconnectClient, this, conn_key).detach();
    }

    void CrossSocketSrvTCP::SendHandler(std::string conn_key, Buffer* recv_buff) //separate thread for each client
    {   
        PRINT_DBG("GET into handler TCP server conn : %s\n", conn_key.c_str());
        auto send_buff = (_received_data_handler != nullptr)? _received_data_handler(&_cw, conn_key, recv_buff) : nullptr;

        if(_send_data && send_buff != nullptr)
        {
            if(Send(_cw.Get_conn_socket(conn_key), send_buff) <= 0)
            {
                _cw.Set_status(conn_key, ConnStatuses::DISCONNECT);
            }
        }

        PRINT_DBG("GET OUT handler TCP server conn : %s\n", conn_key.c_str());

        delete recv_buff;

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