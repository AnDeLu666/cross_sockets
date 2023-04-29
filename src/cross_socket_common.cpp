#include "cross_socket_common.h"

namespace cross_socket
{           
    struct KeepAliveConfig 
    {
        /** The time (in seconds) the connection needs to remain 
         * idle before TCP starts sending keepalive probes (TCP_KEEPIDLE socket option)
         */
        int keepidle;
        /** The maximum number of keepalive probes TCP should 
         * send before dropping the connection. (TCP_KEEPCNT socket option)
         */
        int keepcnt;

        /** The time (in seconds) between individual keepalive probes.
         *  (TCP_KEEPINTVL socket option)
         */
        int keepintvl;
    };


    bool SetSockoptTCPKeepAlive(Socket conn_socket, int flag) 
    {
        //flag 1 turn on 0 turn off
        struct KeepAliveConfig ka_cfg = { 60, 5, 3};
        if (setsockopt(conn_socket, SOL_SOCKET, SO_KEEPALIVE, WIN_(reinterpret_cast<char*>)(&flag), sizeof(flag)) NIX_(== -1)WIN_(!= 0))
        {
            perror("Setsockopt SO_KEEPALIVE");
            return false;
        }
  
        #ifdef _WIN64
            tcp_keepalive ka {1, static_cast<long unsigned int>(ka_cfg.keepidle) * 1000, 
                                static_cast<long unsigned int>(ka_cfg.keepintvl) * 1000
            };
            
            unsigned long numBytesReturned = 0;
            if(WSAIoctl(conn_socket, SIO_KEEPALIVE_VALS, &ka, sizeof (ka), nullptr, 0, &numBytesReturned, 0, nullptr) != 0)
            {
                return false;
            }
        #else //POSIX
            if(setsockopt(conn_socket, IPPROTO_TCP, TCP_KEEPIDLE, &ka_cfg.keepidle, sizeof(ka_cfg.keepidle)) == -1)
            {
                perror("Setsockopt TCP_KEEPIDLE");
                return false;
            }

            if(setsockopt(conn_socket, IPPROTO_TCP, TCP_KEEPCNT, &ka_cfg.keepcnt, sizeof(ka_cfg.keepcnt)) == -1)
            {
                perror("Setsockopt TCP_KEEPCNT");
                return false;
            }
           
            if(setsockopt(conn_socket, IPPROTO_TCP, TCP_KEEPINTVL, &ka_cfg.keepintvl, sizeof(ka_cfg.keepintvl)) == -1)
            {
                perror("Setsockopt TCP_KEEPINTVL");
                return false;
            }
        #endif

        return true;
    }

    //flag 0 blocked 1 non block mode 
    void SetWIN_TCPNonBlockingTCPSocket(Socket socket)
    {
        //turn on non blocking mode for socket in Windows
        WIN_(
        u_long flag = 1;
        if( SOCKET_ERROR == ioctlsocket(socket, FIONBIO, &flag))
        { 
            PRINT_DBG("__SOCK_ERROR_\n");
        });
    }

    int GetSockoptError(Socket socket)
    {   

        int error = 0;

        Socklen_t len = sizeof(error);
        
        getsockopt(socket, SOL_SOCKET, SO_ERROR, WIN_((char*))&error, &len);

        if(!error)
        {
            error = errno;
        }

        switch(error)
        {
            case 0:
                break;

            NIX_(
            case ECONNABORTED:
            case ECONNRESET:
            case EPIPE:
            case ETIMEDOUT:
            )
#ifdef _WIN64
            case WSAECONNRESET:
            case WSAECONNABORTED:
            case WSAETIMEDOUT:
#endif
                PRINT_DBG("--------------->>>>Socket error %d <<<\n", error); 
                break;

            //   EAGAIN or EWOULDBLOCK
            //   The socket's file descriptor is marked O_NONBLOCK and no data is
            //   waiting  to  be  received;  or MSG_OOB is set and no out-of-band
            //   data is available and either the  socket's  file  descriptor  is
            //   marked  O_NONBLOCK  or  the  socket does not support blocking to
            //   await out-of-band data.
            NIX_(
            case EAGAIN: //EWOULDBLOCK
            )
#ifdef _WIN64
            case WSAEWOULDBLOCK:
#endif
                error = 0;// now cross_sockets ignore it because works in non blocking mode 
                break;
            
            default:
                PRINT_DBG("--------------->>>>Extra Socket error %d <<<\n", error); 

                break;
        }  

        return error;
    }

    SocketError Server_InitTCP(Socket socket, uint16_t port, struct sockaddr_in& address)
    {
        SocketError error_ = SocketError::NO_ERRORS;
        int flag = 1;

        if(!SetSockoptTCPKeepAlive(socket, flag))
        {
            perror("SOCK_SETOPT_ERROR");
            return  SocketError::SOCK_SETOPT_ERROR;
        }

        if ((error_ = Server_Bind(socket, port, address)) != SocketError::NO_ERRORS)
        {
            return error_;
        }

        if (listen(socket, 3) < 0)
        {
            perror("Socket listening failed");
            return SocketError::SOCK_LISTEN_ERROR;
        }

        return error_;
    }

    SocketError Server_Bind(Socket socket, uint16_t port, sockaddr_in& address)
    {
        address.sin_port = htons(port);

        // Forcefully attaching socket to a port
        if (bind(socket, reinterpret_cast<sockaddr *>(&address), sizeof(address)) < 0)
        {
            perror("Socket binding failed");
            return SocketError::SOCK_BIND_ERROR;
        }
        else
        {
            return SocketError::NO_ERRORS;
        }
    }


} // end namespace cross_socket