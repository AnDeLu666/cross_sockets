#pragma once

#include "cross_socket.h"

namespace cross_socket
{

class CrossSocketSrv: public CrossSocket
{
private:
    int _port;

    std::thread _accept_thread;

    void AcceptConnection();

public:
    CrossSocketSrv(int socket_type, unsigned int port);

    void ConnectionHandler(std::string index);
    
    void Start();
    void Stop();

    ~CrossSocketSrv();
};

}//end namespace cross_socket
