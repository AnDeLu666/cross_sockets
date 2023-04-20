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

        if ((_sock_error = cross_socket::Server_Bind(_socket, _port, _srv_addr)) == SocketError::EMPTY)
        {           
            _accept_thread = std::thread(&CrossSocketSrvUDP::AcceptHandler, this);
            _accept_thread.detach();

            _status = SrvStatuses::STARTED;
        }
    }


    void CrossSocketSrvUDP::AcceptHandler()
    {
        cross_socket::Buffer recv_buff{};
        cross_socket::Buffer* send_buff;

        while (_status != SrvStatuses::STOP)
        {
            recv_buff = Recv(_socket, &_address);
            if (recv_buff.real_bytes <= 0)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
            else
            {
                uint16_t port = htons(_address.sin_port);
                std::string conn_indx = inet_ntoa((&_address)->sin_addr);
                conn_indx += ":" + std::to_string(port);
                    
                if(_connections.find(conn_indx) == _connections.end()) //TODO protection network atacks
                {
                    _connections[conn_indx] = std::make_shared<Connection>(_socket);
                }

                send_buff = _main_handler_ptr(_connections[conn_indx], recv_buff);

                Send(_socket, send_buff, &_address);
            }

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