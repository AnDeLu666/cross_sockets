#include "cross_socket_srv_udp.h"

//temporary
#include <cstring>

namespace cross_socket
{
    CrossSocketSrvUDP::CrossSocketSrvUDP(unsigned int port)
    : CrossSocketSrv(port), CrossSocket(UDP) 
    {}


    void CrossSocketSrvUDP::ConnectionHandler(std::string index)
    {
        while(_continue_work)
        {
            if(Recv(index, _address) > 0)
            {
                PRINT_DBG("received from client : %s %d\n", _connections[index]->GetBufferFrom().data, _connections[index]->_conn_socket);
                _connections[index]->SetBufferTo((char*)"\37", 1);

                Send(index, _address);
                
                if(std::strcmp(_connections[index]->GetBufferFrom().data, "exit1") == 0)
                {
                    PRINT_DBG("break\n");
                    break;
                }
            }
            else
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        }

    }

    void CrossSocketSrvUDP::Start()
    {
        _address.sin_port = htons(_port);

        _srv_addr.sin_family    = AF_INET; // IPv4
        _srv_addr.sin_addr.s_addr = INADDR_ANY;

        if((_sock_error = cross_socket::Server_Bind(_socket, _port, _srv_addr)) == SocketError::EMPTY)
        {
            std::string index = std::to_string(_socket); //TODO if more 1 client (now only 1 socket and connection)
        
            _connections[index] = std::make_shared<Connection>(_socket);
            _connections[index]->_thread = std::thread(&CrossSocketSrvUDP::ConnectionHandler, this, index);
            _connections[index]->_thread.detach();
        }
    }

} // end namespace cross_socket