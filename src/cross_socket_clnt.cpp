#include "cross_socket_clnt.h"

namespace cross_socket
{

    CrossSocketClnt::CrossSocketClnt(unsigned int port)
    : CrossSocket(port)
    {}

    void CrossSocketClnt::Connect()
    {
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
            _conn_s = _socket;
            _status = CONNECTED;
        }
    }

    void CrossSocketClnt::Disconnect()
    {

    }

    CrossSocketClnt::~CrossSocketClnt()
    {
        this->Disconnect();
        this->CloseSocket();
        delete[] _buffer;
    }

} //end namespace cross_socket