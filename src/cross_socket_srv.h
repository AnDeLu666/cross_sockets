#pragma once

#include "cross_socket.h"

namespace cross_socket
{

class CrossSocketSrv: public CrossSocket
{
private:
    std::thread _accept_thread;
    void AcceptConnection();

public:
    CrossSocketSrv(unsigned int port);

    void Start();
    void Stop();

    ~CrossSocketSrv();
};

}//end namespace cross_socket
