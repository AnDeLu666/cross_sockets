#pragma once

#include "cross_socket.h"
#include "cross_socket_conn_wrapper.h"

namespace cross_socket
{
    struct SrvStatuses
    {
        enum SrvStatus
        {
            NEW,
            STARTED,
            STOP
        };
    };

    class CrossSocketSrv
    {
        protected:
            std::thread _accept_thread;
            std::function<Buffer*(cross_socket::ConnectionsWrapper* cw, std::string conn_key, Buffer&)> _main_handler_ptr = nullptr;

            struct sockaddr_in _address;
            uint16_t _port;
            
            virtual void Start_() = 0;

            virtual void AcceptHandler() = 0;
            virtual void MainHandler(std::string index) = 0;

        public:
            ConnectionsWrapper _cw;  // connections wrapper  
            
            SrvStatuses::SrvStatus _status = SrvStatuses::NEW;
            
            CrossSocketSrv(uint16_t port);

            bool Start();
            
            void Stop();

            void Set_main_handler_ptr(std::function<Buffer*(cross_socket::ConnectionsWrapper* cw, std::string conn_key, Buffer&)> func_ptr);

            ~CrossSocketSrv();
    };

}//end namespace cross_socket
