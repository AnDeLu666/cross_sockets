#include "cross_socket_srv_udp.h"

namespace cross_socket
{
    CrossSocketSrvUDP::CrossSocketSrvUDP(uint16_t port)
        : CrossSocketSrv(port), CrossSocket(UDP, true)
    {
    }

    void CrossSocketSrvUDP::Start_()
    {
        _srv_addr.sin_family = AF_INET; // IPv4
        _srv_addr.sin_addr.s_addr = INADDR_ANY;

        if ((_sock_error = cross_socket::Server_Bind(_socket, _port, _srv_addr)) == SocketError::NO_ERRORS)
        {
            _accept_thread = std::thread(&CrossSocketSrvUDP::AcceptHandler, this);

            _status = SrvStatuses::STARTED;
        }
    }

    void CrossSocketSrvUDP::AcceptHandler()
    {
        const short sleep_time_limit = 100;
        const short sleep_time_min = 1;
        short sleep_time = sleep_time_min; //milliseconds it increases if not receive anything up to sleep_time_limit

        //cross_socket::Buffer *recv_buff;
        byte_t* segment_buffer = new byte_t[DEFAULT_MSS]{0};

        while (_status != SrvStatuses::STOP)
        {
            //recv_buff must be initialized or use recv_buff = Recv(..., nullptr instead of recv_buff)
            //nullptr - when received data creates new buffer
            auto recv_buff = Recv(_socket, _address, segment_buffer, nullptr); 
            if (GetSockoptError(_socket) != SocketError::NO_ERRORS)
            {
                auto conn_key = GetIP_PortStringFromAddress(_address);

                if (_cw.Find(conn_key))
                {
                    _cw.DeleteConnection(conn_key);
                }

                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                
                break;
            }

            if(recv_buff != nullptr)
            {
                if (recv_buff->real_bytes > 0)
                {
                    sleep_time = sleep_time_min;

                    auto conn_key = GetIP_PortStringFromAddress(_address);

                    Socket conn_socket = _socket; // TODO get separate port and socket in a new thread

                    if (!_cw.Find(conn_key)) // TODO protection network atacks
                    {
                        _cw.AddNewConnection(conn_key, conn_socket);

                        sockaddr_in address{_address};
                        _cw.Set_address_ref(conn_key, address);
                    }
                    
                    if(_received_data_handler != nullptr)
                    {
                        if(recv_buff->real_bytes < MAX_RECV_SIZE_THIS_HANDLER) //continue here
                        {
                            SendHandler(conn_key, recv_buff);
                        }
                        else //start a new thread
                        {
                            std::thread(&CrossSocketSrvUDP::SendHandler, this, conn_key, recv_buff).detach();
                        }
                    }
                }
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
            sleep_time = (sleep_time < sleep_time_limit) ? sleep_time + 1 : sleep_time_limit;
        }
        
        delete[] segment_buffer;
    }

    void CrossSocketSrvUDP::SendHandler(std::string conn_key, Buffer *recv_buff) // separate thread for each client
    {
        PRINT_DBG("GET INTO UDP SRV mainhandler_ptr conn : %s\n", conn_key.c_str());
        auto send_buff = (_received_data_handler != nullptr)? _received_data_handler(&_cw, conn_key, recv_buff) : nullptr;

        if(_send_data && send_buff != nullptr)
        {
            if (Send(_cw.Get_conn_socket(conn_key), send_buff, _cw.Get_address_ref(conn_key)) <= 0)
            {
                _cw.DeleteConnection(conn_key);
            }
        }
        
        delete recv_buff;
        PRINT_DBG("GET OUT UDP SRV mainhandler_ptr conn : %s\n", conn_key.c_str());
    }

    CrossSocketSrvUDP::~CrossSocketSrvUDP()
    {
        PRINT_DBG("udp class srv_destr \n");
    }

} // end namespace cross_socket