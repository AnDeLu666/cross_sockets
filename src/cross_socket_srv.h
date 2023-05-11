#pragma once

#include "cross_socket.h"

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
            
            struct sockaddr_in _address;
            uint16_t _port;
            
            virtual void Start_() = 0;

            virtual void AcceptHandler() = 0;
            virtual void SendHandler(std::string conn_key, Buffer* recv_buf) = 0;

        public:
            SrvStatuses::SrvStatus _status = SrvStatuses::NEW;
            
            CrossSocketSrv(uint16_t port);

            bool Start();
            
            void Stop();

            ~CrossSocketSrv();
    };

}//end namespace cross_socket
