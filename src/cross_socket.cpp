#include "cross_socket.h"

namespace cross_socket
{
    Socket CrossSocket::InitNewSocket()
    {
        Socket socket_tmp = -20;

        // Creating socket file descriptor
        if ((socket_tmp = socket(AF_INET, _socket_type, 0)) < 0)
        {
            perror("Socket init failure");
            _sock_error = SocketError::SOCK_INIT_ERROR;
        }

        // flag 1 turn on 0 turn off
        int flag = 1;

        if (setsockopt(socket_tmp, SOL_SOCKET, SO_REUSEADDR NIX_(| SO_REUSEPORT),
                       WIN_(reinterpret_cast<char *>)(&flag), sizeof(flag)))
        {
            perror("Setsockopt failure");
            return SocketError::SOCK_SETOPT_ERROR;
        }

        if (_socket_type == UDP)
        {
            SetSockOptTimeout(_socket, DEFAULT_SOCKET_TIMEOUT); // set 1 s timeout - sometimes socket hungs
        }

        return socket_tmp;
    }

    CrossSocket::CrossSocket(int socket_type, bool init_socket = true)
        : _socket_type(socket_type)
    {
        if (_socket_type != TCP && _socket_type != UDP)
        {
            _sock_error = SocketError::SOCK_UNKNOWN_TYPE;
        }
        else
        {
            WIN_(WSADATA WSAData;)                      // some data
            WIN_(WSAStartup(MAKEWORD(2, 0), &WSAData);) // initialize sockets

            if (init_socket)
            {
                _socket = InitNewSocket();

                PRINT_DBG("socket type : %s %d\n", (_socket_type == TCP) ? "TCP" : "UDP", (int)_socket);
            }
        }
    }

    void CrossSocket::SetSockOptTimeout(Socket socket, short recv_send_timeout_sec) // I guess it lost effect in non blocking mode TODO test
    {
        if (recv_send_timeout_sec > 0)
        {
            // LINUX, MAC OS X
            NIX_(struct timeval timeout;
                 timeout.tv_sec = recv_send_timeout_sec;
                 timeout.tv_usec = 0;);

            // WINDOWS
            WIN_(DWORD timeout = recv_send_timeout_sec * 1000;)

            setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const char *>(&timeout), sizeof timeout);
        }
    }

    SocketError CrossSocket::GetSockError()
    {
        return _sock_error;
    }

    cross_socket::Buffer* CrossSocket::Recv(Socket recv_sock, sockaddr_in &address)
    {
        Buffer *buff = new cross_socket::Buffer{};

        if (recv_sock < 0)
        {
            return buff;
        }

        Socklen_t len = sizeof(address);

        My_ssize_t recv_bytes = 0;

        buff->real_bytes = 0;

        byte_t *buffer;

        data_size_t buff_size = DEFAULT_BUFFER_SIZE;

        // todo start time after change all to async
        do
        {
            recv_bytes = 0;
            buffer = new byte_t[buff_size]{'\0'};

            // recieve data
            if (_socket_type == TCP)
            {
                recv_bytes = recv(recv_sock, WIN_(reinterpret_cast<char *>)(buffer), buff_size, 0 NIX_(| MSG_DONTWAIT));
            }
            else if (_socket_type == UDP)
            {
                // winsocks do not support MSG_WAITALL for UDP(SOCK_DGRAM) sockets
                recv_bytes = recvfrom(recv_sock, WIN_(reinterpret_cast<char *>)(buffer), buff_size, 0 NIX_(| MSG_WAITALL),
                                        reinterpret_cast<sockaddr *>(&address), &len);
            }

            if (recv_bytes > 0 && recv_bytes <= buff_size)
            {
                buff->data.insert(buff->data.end(), buffer, buffer + recv_bytes);
                buff->real_bytes += recv_bytes;
                PRINT_DBG("recieved bytes %lld buffer size %d \n", buff->real_bytes, buff_size);
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(1));

            delete[] buffer;

        } while (recv_bytes > 0);

        return buff;
    }

    int CrossSocket::Send(Socket send_sock, Buffer &buff, sockaddr_in &address)
    {
        if (send_sock < 0)
        {
            return 0;
        }

        Socklen_t len = sizeof(address);

        My_ssize_t sent_bytes = 0;
        buff.real_bytes = 0;

        if (buff.data.size() <= MAX_RECV_SEND_DATA_SIZE) //MAX_RECV_SEND_DATA_SIZE must  be related to data_size_t
        {
            data_size_t data_size = buff.data.size();

            // test_use
            // PRINT_DBG("list of accepted values\n");
            // PRINT_DBG("data_size : %d \n", data_size);
            // for(const byte_t x: buff.data)
            //    PRINT_DBG("%c", char(x));
            // PRINT_DBG("\n");

            if (data_size == 0)
            {
                buff.data.emplace_back(0);
                data_size = buff.data_size();
            }

            do
            {
                sent_bytes = 0;

                // send data
                if (_socket_type == TCP)
                {
                    // if do not set MSG_NOSIGNAL send will throw exception
                    sent_bytes = send(send_sock, WIN_(reinterpret_cast<char *>)(&buff.data[buff.real_bytes]),
                                        data_size - buff.real_bytes, NIX_(MSG_DONTWAIT| MSG_NOSIGNAL) WIN_(0));
                }
                else if (_socket_type == UDP)
                {
                    // winsocks do not support MSG_WAITALL for UDP(SOCK_DGRAM) sockets
                    sent_bytes = sendto(send_sock, WIN_(reinterpret_cast<char *>)(&buff.data[buff.real_bytes]),
                                        data_size - buff.real_bytes, 0 NIX_(| MSG_WAITALL), reinterpret_cast<sockaddr *>(&address), len);
                }

                if (sent_bytes > 0)
                {
                    buff.real_bytes += sent_bytes;
                }
                else
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(1));
                }
                PRINT_DBG("________________inside sent_bytes : %lli \n", sent_bytes);

            } while (sent_bytes > 0);
            
        }

        return buff.real_bytes;
    }


    CrossSocket::~CrossSocket()
    {
#ifdef _WIN64
        closesocket(_socket);
        WSACleanup();
#else
        close(_socket); // close main(listen) socket
        shutdown(_socket, SHUT_RDWR);
#endif
        PRINT_DBG("CrossSocket destr \n");
    }

} // end namespace cross_socket