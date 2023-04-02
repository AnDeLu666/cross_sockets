#include "cross_socket_common.h"
#include <vector>

namespace cross_socket
{
    // begin Servers init ------------------------------------
    SocketError Server_InitTCP(int socket, int port, int opt, struct sockaddr_in& address)
    {
        SocketError error_ = SocketError::EMPTY;

        // Set socket options
        if (setsockopt(socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                       &opt, sizeof(opt)))
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

    SocketError Server_Bind(int socket, int port, sockaddr_in& address)
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

    int TCP_Send(std::shared_ptr<Connection> cur_conn)
    {
        struct Buffer *buff = cur_conn->GetBufferTo();

        PRINT_DBG("data -------------------- %s \n", buff->data);

        PRINT_DBG("data size-------------------- %d \n", buff->size);

        // send data size  if sent_bytes 0 - client has been disconnecter -1 error
        int sent_bytes = send(cur_conn->_conn_socket, (const char *)&buff->size, sizeof(u_int32_t), 0);

        PRINT_DBG("sent_bytes 1st  %d \n", sent_bytes);

        if (sent_bytes > 0)
        {
            if (buff->size > 0)
            {
                // send data
                sent_bytes = send(cur_conn->_conn_socket, buff->data, buff->size, 0);
            }
        }

        PRINT_DBG("sent_bytes %d \n", sent_bytes);

        return sent_bytes;
    }

    int TCP_Recv(std::shared_ptr<Connection> cur_conn)
    {
        u_int32_t data_size = 0; // no more then 4 bytes

        // receive data size if recieved_bytes 0 - client has been disconnecter -1 error
        int recieved_bytes = read(cur_conn->_conn_socket, (char *)&data_size, sizeof(u_int32_t));

        PRINT_DBG("recieved bytes data size%d ds %d \n", recieved_bytes, data_size);

        if (recieved_bytes > 0)
        {
            if (data_size > 0)
            {
                cur_conn->_buffer_from.data = new char[data_size + 1]{'\0'};
                cur_conn->_buffer_from.size = data_size;

                // recieve data
                recieved_bytes = read(cur_conn->_conn_socket, cur_conn->_buffer_from.data, data_size);

                PRINT_DBG("recieved bytes %d size %d \n", recieved_bytes, data_size);
            }
        }

        return recieved_bytes;
    }

} // end namespace cross_socket