#include "cross_socket_clnt_udp.h"

namespace cross_socket
{
    CrossSocketClntUDP::CrossSocketClntUDP()
    : CrossSocketClnt(UDP, false)
    {}

    void CrossSocketClntUDP::SendHandler(std::string conn_key)
    {   
        cross_socket::Buffer* recv_buff;
        const short sleep_time_limit = 10;
        short sleep_time = 1; //milliseconds it increases if not receive anything up to 

        while(_cw.Get_status(conn_key) == ConnStatuses::CONNECTED)
        {     
            if(!_send_data || !_cw.If_send_buffer_is_nullptr(conn_key))
            {
                if(!_send_data || (Send(_cw.Get_conn_socket(conn_key), &_cw.Get_send_buffer_ref(conn_key), _cw.Get_address_ref(conn_key)) > 0))
                {
                    if(_send_data)
                    {
                        _cw.Set_send_buffer_ptr(conn_key, nullptr); //free memory
                    }

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

                    if(_received_data_handler != nullptr)
                    {
                        _received_data_handler(&_cw, conn_key, recv_buff);
                    }

                    sleep_time = 1;
                    delete recv_buff;
                    
                }
                else
                {
                    if(_send_data)
                    {
                        _cw.Set_status(conn_key, ConnStatuses::DISCONNECT);
                    }
                }
                
                _cw.Set_send_buffer_ptr(conn_key, nullptr); 
            }
 
            std::this_thread::sleep_for(std::chrono::milliseconds(sleep_time));
            sleep_time = (sleep_time < sleep_time_limit) ? sleep_time + 1 : sleep_time_limit;

        }

        //starting a new thread to destroy connection
        std::thread(&CrossSocketClnt::Disconnect, this, conn_key).detach();
    }


    CrossSocketClntUDP::~CrossSocketClntUDP()
    {
        PRINT_DBG("CrossSocketClntUDP destr \n");
    }


} //end namespace cross_socket