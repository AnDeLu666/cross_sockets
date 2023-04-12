#include "cross_socket_common.h"

namespace cross_socket
{
    // begin Servers init ------------------------------------
    SocketError Server_InitTCP(Socket socket, uint16_t port, int opt, struct sockaddr_in& address)
    {
        SocketError error_ = SocketError::EMPTY;

        // Set socket options TODO learn more
        if (setsockopt(socket, SOL_SOCKET, SO_REUSEADDR NIX_(| SO_REUSEPORT),
                       WIN_((char*))&opt, sizeof(opt)))
        {
            perror("Setsockopt failure");
            error_ = SocketError::SOCK_SETOPT_ERROR;
        }

        error_ = Server_Bind(socket, port, address);

        if (listen(socket, 3) < 0)
        {
            perror("Socket listening failed");
            error_ = SocketError::SOCK_LISTEN_ERROR;
        }

        return error_;
    }

    SocketError Server_Bind(Socket socket, uint16_t port, sockaddr_in& address)
    {
        address.sin_port = htons(port);

        // Forcefully attaching socket to a port
        if (bind(socket, (struct sockaddr *)&address, sizeof(address)) < 0)
        {
            perror("Socket binding failed");
            return SocketError::SOCK_BIND_ERROR;
        }
        else
        {
            return SocketError::EMPTY;
        }
    }
    // end Servers init --------------------------------------

} // end namespace cross_socket