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
        cross_socket::Buffer* recv_buff;
        cross_socket::Buffer* send_buff;

        while (_status != SrvStatuses::STOP)
        {
            recv_buff = Recv(_socket, _address);
            if (GetSockoptError(_socket) != SocketError::NO_ERRORS)
            {
                break;
            }
            if (recv_buff->real_bytes > 0)
            {
                uint16_t port = htons(_address.sin_port);
                std::string conn_key = inet_ntoa((&_address)->sin_addr);
                conn_key += ":" + std::to_string(port);

                Socket conn_socket = _socket; //TODO get separate port and socket in a new thread
                    
                if(!_cw.Find(conn_key)) //TODO protection network atacks
                {
                    _cw.AddNewConnection(conn_key, conn_socket);
                }

                send_buff = _main_handler_ptr(&_cw, conn_key, *recv_buff);

                if(Send(_cw.Get_conn_socket(conn_key), *send_buff, _address) <= 0)
                {
                    _cw.Set_status(conn_key, ConnStatuses::DISCONNECT);
                }
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(1));

            recv_buff = {};
        }
    }

    void CrossSocketSrvUDP::MainHandler(std::string index) //separate thread for each client
    {
        //todo someday
        while (_status != SrvStatuses::STOP)
        {

        }
    }

    CrossSocketSrvUDP::~CrossSocketSrvUDP()
    {
        PRINT_DBG("udp class srv_destr \n");
    }

} // end namespace cross_socket