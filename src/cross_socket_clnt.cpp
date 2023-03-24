#include "cross_socket_clnt.h"

namespace cross_socket
{

    CrossSocketClnt::CrossSocketClnt()
    : CrossSocket(0)
    {}

    void CrossSocketClnt::ConnectionHandler(const char* data) //(Connection *srv, const char* data)
    {
        std::shared_ptr<Connection> srv = _connections["0"];
        printf("sent to server bytes %d\n",  srv->Send(data)); 

        srv->Recv();
    }

    void CrossSocketClnt::Connect(unsigned int port)
    {
        _port = port;

        _address.sin_port = htons(_port);

        //convert IPv4 and IPv6 addresses from text to binary 
        if(inet_pton(AF_INET, "127.0.0.1", &_address.sin_addr) < 0)
        {
            perror("Invalid address/ Address not supported\n");
            _status = INVALID_IP_ERROR;
        }

        if(connect(_socket, (struct sockaddr*)&_address, sizeof(_address)) < 0)
        {
            perror("Connection failed\n");
            _status = CONNECTION_ERROR;
        }
        else
        {
            _status = CONNECTED;
            
            _connections["0"] = std::make_shared<Connection>(_socket);
            //_connections["0"]->thread_id = std::thread(&CrossSocketClnt::ConnectionHandler, this->_connections["0"]);
            //_connections["0"]->thread_id.detach();
        }
    }

    void CrossSocketClnt::Disconnect()
    {

    }

    CrossSocketClnt::~CrossSocketClnt()
    {
        ConnectionsMap::iterator it = _connections.begin();
        for( ; it != _connections.end(); it++)
        {
            printf("con %s \n", it->first);
        }

        printf("cltd_destr \n");
        this->Disconnect();
    }

} //end namespace cross_socket