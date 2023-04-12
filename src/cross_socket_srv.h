#pragma once

#include "cross_socket.h"

namespace cross_socket
{
    enum SrvStatuses
    {
        NEW,
        STARTED,
        STOP
    };

    class CrossSocketSrv
    {
        protected:
            std::thread _accept_thread;
            std::function<cross_socket::Buffer(std::string conn_indx, cross_socket::Buffer&)> _main_handler_ptr = nullptr;
            std::function<bool(cross_socket::Buffer&)> _auth_handler_ptr = [](cross_socket::Buffer& buff){ return true;};

            uint16_t _port;
            
            virtual void Start_() = 0;

            virtual void AcceptHandler() = 0;
            virtual void MainHandler(std::string index) = 0;

        public:
            SrvStatuses _status = SrvStatuses::NEW;
            
            CrossSocketSrv(uint16_t port);

            bool Start();
            void Stop();

            void Set_auth_handler_ptr(std::function<bool(cross_socket::Buffer&)> func_ptr);
            void Set_main_handler_ptr(std::function<cross_socket::Buffer(std::string conn_indx, cross_socket::Buffer&)> func_ptr);

            ~CrossSocketSrv();
    };

}//end namespace cross_socket
