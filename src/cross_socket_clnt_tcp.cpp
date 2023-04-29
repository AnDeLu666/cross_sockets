#include "cross_socket_clnt_tcp.h"

namespace cross_socket
{
    CrossSocketClntTCP::CrossSocketClntTCP()
    : CrossSocketClnt(TCP, false)
    {}

    void CrossSocketClntTCP::MainHandler(std::string conn_key) 
    {
        //todo move to connection after delete classes tcp and udp and keeps only clnt
        SetWIN_TCPNonBlockingTCPSocket(_cw.Get_conn_socket(conn_key)); //under linux it sets in recv send

        cross_socket::Buffer* recv_buff;

        while(_cw.Get_status(conn_key) == ConnStatuses::CONNECTED)//TODO status == connected
        {       
            const short sleep_time_limit = 10;
            short sleep_time = 1; //milliseconds it increases if not receive anything up to 

            if(!_cw.If_send_buffer_is_nullptr(conn_key))
            {
                if(Send(_cw.Get_conn_socket(conn_key), _cw.Get_send_buffer_ref(conn_key), _cw.Get_address_ref(conn_key)) > 0)
                {
                    do
                    {
                        recv_buff = Recv(_cw.Get_conn_socket(conn_key), _cw.Get_address_ref(conn_key));
                        if (GetSockoptError(_cw.Get_conn_socket(conn_key)) != SocketError::NO_ERRORS)
                        { 
                            _cw.Set_status(conn_key, ConnStatuses::DISCONNECT);
                            break;
                        }

                        if(recv_buff->real_bytes > 0)
                        {
                            sleep_time = 1;
                        }
                        else
                        {
                            delete recv_buff;
                            recv_buff = nullptr;
                        }

                        std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));

                        sleep_time = (sleep_time < sleep_time_limit) ? sleep_time + 1 : sleep_time_limit;

                    } while (recv_buff == nullptr || recv_buff->real_bytes <= 0);

                    //todo pass recv data to data_handler_func
                    sleep_time = 1;
                    delete recv_buff;
                    
                }
                else
                {
                    _cw.Set_status(conn_key, ConnStatuses::DISCONNECT);
                }
                
                _cw.Set_send_buffer_ptr(conn_key, nullptr); 
            }
 
            std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
            sleep_time = (sleep_time < sleep_time_limit) ? sleep_time + 1 : sleep_time_limit;

        }

        //starting a new thread to destroy connection
        std::thread(&CrossSocketClnt::Disconnect, this, conn_key).detach();

    }


    CrossSocketClntTCP::~CrossSocketClntTCP()
    {   
        PRINT_DBG("CrossSocketClntTCP destr \n");
    }

} //end namespace cross_socket