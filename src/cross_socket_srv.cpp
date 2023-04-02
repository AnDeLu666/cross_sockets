#include "cross_socket_srv.h"

namespace cross_socket
{
    CrossSocketSrv::CrossSocketSrv(unsigned int port)
    :_port(port)
    {}

    void CrossSocketSrv::Stop()
    {
        if (_accept_thread.joinable())
        {
            _accept_thread.join();
        }
    }

    CrossSocketSrv::~CrossSocketSrv()
    {
        PRINT_DBG("srv_destr \n");
        this->Stop();
    }

} // end namespace cross_socket