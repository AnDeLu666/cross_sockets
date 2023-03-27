#include "cross_socket_clnt.h"

namespace cross_socket
{

    CrossSocketClnt::CrossSocketClnt()
    : CrossSocket(0)
    {}

    void CrossSocketClnt::ConnectionHandler(std::string index) //, char* data) 
    {
        std::string data;

        printf("index = %s enter command : \n", index.c_str());
        
        while(data != "exit")
        {
            std::getline(std::cin, data);

            if(_connections[index]->Send(data.c_str()) > 0)
            {
                if(_connections[index]->Recv() <= 0)
                {
                    perror("Server isn't available\n");      
                } 
            }
        }

        _status = cross_socket::STOP;
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
            
            std::string index = std::to_string(_socket);
            _connections[index] = std::make_shared<Connection>(_socket);
            _connections[index]->_thread = std::thread(&CrossSocketClnt::ConnectionHandler, this, index);
            _connections[index]->_thread.detach();
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
            printf("con %s \n", it->first.c_str());
        }

        printf("cltd_destr \n");
        this->Disconnect();
    }

} //end namespace cross_socket