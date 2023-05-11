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
        cross_socket::Buffer *recv_buff;

        while (_status != SrvStatuses::STOP)
        {
            recv_buff = Recv(_socket, _address);
            if (GetSockoptError(_socket) != SocketError::NO_ERRORS)
            {
                auto conn_key = GetIP_PortStringFromAddress(_address);

                if (_cw.Find(conn_key))
                {
                    _cw.DeleteConnection(conn_key);
                }

                std::this_thread::sleep_for(std::chrono::milliseconds(1));
                
                delete recv_buff;
                
                break;
            }

            if (recv_buff->real_bytes > 0)
            {
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
                    if(recv_buff->data_size() < MAX_RECV_SIZE_THIS_HANDLER) //continue here
                    {
                        SendHandler(conn_key, recv_buff);
                    }
                    else //start a new thread
                    {
                        std::thread(&CrossSocketSrvUDP::SendHandler, this, conn_key, recv_buff).detach();
                    }
                }
            }
            else
            {
                delete recv_buff;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
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

        PRINT_DBG("GET OUT UDP SRV mainhandler_ptr conn : %s\n", conn_key.c_str());
        delete recv_buff; // n=but it automaticaly should be deleted
    }

    CrossSocketSrvUDP::~CrossSocketSrvUDP()
    {
        PRINT_DBG("udp class srv_destr \n");
    }

} // end namespace cross_socket