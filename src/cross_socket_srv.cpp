#include "cross_socket_srv.h"

namespace cross_socket
{
    CrossSocketSrv::CrossSocketSrv(uint16_t port)
    :_port(port)
    {}

    bool CrossSocketSrv::Start()
    {
        if(_status == SrvStatuses::NEW)
        {
            _address.sin_family    = AF_INET; // IPv4 //structure for client
            _address.sin_addr.s_addr = INADDR_ANY;

            Start_();
            
            return true; //todo check if errors
        }
        
        return false;
    }

    void CrossSocketSrv::Stop()
    {
        if (_accept_thread.joinable())
        {
            _accept_thread.join();
        }
    }

    void CrossSocketSrv::Set_auth_handler_ptr(std::function<bool(cross_socket::Buffer&)> func_ptr)
    {
        _auth_handler_ptr = func_ptr;
    }

    void CrossSocketSrv::Set_main_handler_ptr(std::function<cross_socket::Buffer(std::string conn_indx, cross_socket::Buffer&)> func_ptr)
    {
        _main_handler_ptr = func_ptr;
    }

    CrossSocketSrv::~CrossSocketSrv()
    {
        PRINT_DBG("base class srv_destr \n");
        Stop();
    }
} // end namespace cross_socket