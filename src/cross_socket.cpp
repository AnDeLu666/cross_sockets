#include "cross_socket.h"

namespace cross_socket
{
    CrossSocket::CrossSocket(int socket_type)
    : _socket_type(socket_type)
    {   
        if(socket_type != TCP && socket_type != UDP)
        {
            _sock_error = SocketError::SOCK_UNKNOWN_TYPE;
        }
        else
        {
            PRINT_DBG("socket type : %s \n", (_socket_type == TCP)? "TCP" : "UDP");
            // Creating socket file descriptor
            if((_socket = socket(AF_INET,_socket_type, 0)) < 0)
            {
                perror("Socket init failure");
                _sock_error = SocketError::SOCK_INIT_ERROR;
            }

            //using in all types of servers and udp client
            _address.sin_family    = AF_INET; // IPv4
            _address.sin_addr.s_addr = INADDR_ANY;
        }
    }

    SocketError CrossSocket::GetSockError()
    {
        return _sock_error;
    }

    int CrossSocket::Recv(std::string conn_indx, struct sockaddr_in& address)
    {
        int recieved_bytes = 0;

        u_int32_t data_size = 0; // no more then 4 bytes

        socklen_t len = sizeof(address);

        auto conn = _connections[conn_indx];
                
        // receive data size if recieved_bytes 0 - client has been disconnecter -1 error
        if(_socket_type == TCP)
        {
            recieved_bytes = read(conn->_conn_socket, (char *)&data_size, sizeof(u_int32_t));
        } 
        else if(_socket_type == UDP)
        {
            recieved_bytes = recvfrom(conn->_conn_socket, (char *)&data_size, sizeof(u_int32_t), 
                                        MSG_WAITALL, ( struct sockaddr *) &address, &len);
        }

        if (recieved_bytes > 0)
        {
            PRINT_DBG("recieved bytes data size%d ds %d \n", recieved_bytes, data_size);
            
            if (data_size > 0)
            {
                conn->_buffer_from.data = new char[data_size + 1]{'\0'};
                conn->_buffer_from.size = data_size;

                // recieve data 
                if(_socket_type == TCP)
                {
                    recieved_bytes = read(conn->_conn_socket, conn->_buffer_from.data, data_size);
                } 
                else if(_socket_type == UDP)
                {
                    recieved_bytes = recvfrom(conn->_conn_socket, conn->_buffer_from.data, data_size, 
                                                MSG_WAITALL, ( struct sockaddr *) &address, &len);
                }

                if(recieved_bytes > 0)
                {
                    PRINT_DBG("recieved bytes %d size %d \n", recieved_bytes, data_size);
                }
            }
        }

        return recieved_bytes;
    }

    int CrossSocket::Send(std::string conn_indx, struct sockaddr_in& address)
    {
        int sent_bytes = 0;

        socklen_t len = sizeof(address);

        auto conn = _connections[conn_indx];

        struct Buffer *buff = conn->GetBufferTo();

        PRINT_DBG("data -------------------- %s \n", buff->data);

        PRINT_DBG("data size-------------------- %d \n", buff->size);

        // send data size  if sent_bytes 0 - client has been disconnecter -1 error
        if(_socket_type == TCP)
        {
            PRINT_DBG("tcp %d \n", conn->_conn_socket);
            sent_bytes = send(conn->_conn_socket, (const char *)&buff->size, sizeof(u_int32_t), 0);
        }
        else if(_socket_type == UDP)
        {
            sent_bytes = sendto(conn->_conn_socket, (const char *)&buff->size, sizeof(u_int32_t), 
                                 MSG_CONFIRM, (const struct sockaddr *)&address, len);
        }


        PRINT_DBG("sent_bytes 1st  %d \n", sent_bytes);

        if (sent_bytes > 0)
        {
            if (buff->size > 0)
            {
                // send data
                if(_socket_type == TCP)
                {
                    sent_bytes = send(conn->_conn_socket, buff->data, buff->size, 0);
                }
                else if(_socket_type == UDP)
                {
                    sent_bytes = sendto(conn->_conn_socket, buff->data, buff->size, 
                                        MSG_CONFIRM, (const struct sockaddr *)&address, len);
                }
            }
        }

        PRINT_DBG("sent_bytes %d \n", sent_bytes);

        return sent_bytes;
    }

} //end namespace cross_socket