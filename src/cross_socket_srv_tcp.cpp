#include "cross_socket_srv_tcp.h"

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
        const short sleep_time_min = 10;
        short sleep_time = sleep_time_min; //milliseconds it increases if not receive anything up to sleep_time_limit

        //int timer = 0;

        auto conn_socket = _cw.Get_conn_socket(conn_key);
        auto conn_address = _cw.Get_address_ref(conn_key);

        cross_socket::Buffer* recv_buff = new Buffer;
        byte_t* segment_buffer = new byte_t[DEFAULT_MSS]{0};

        while (_cw.Get_status(conn_key) == ConnStatuses::CONNECTED)
        {
            //recv_buff must be initialized or use recv_buff = Recv(..., nullptr instead of recv_buff)
            recv_buff = Recv(conn_socket, segment_buffer, recv_buff);

            if (GetSockoptError(conn_socket) != SocketError::NO_ERRORS)
            {
                _cw.Set_status(conn_key, ConnStatuses::DISCONNECT);
            }
            
            if(recv_buff != nullptr)
            {
                if(recv_buff->real_bytes > 0)
                {
                    sleep_time = sleep_time_min;

                    if(_received_data_handler != nullptr)
                    {
                        SendHandler(conn_key, recv_buff);
                    }
                }
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

        delete recv_buff;
        delete[] segment_buffer;

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