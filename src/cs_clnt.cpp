#include <iostream>

#include "cross_socket_clnt_tcp.h"
#include "cross_socket_clnt_udp.h"


int main(int argc, char const* argv[]) 
{   
    
    cross_socket::CrossSocketClntTCP clnt;

    clnt.Connect("127.0.0.1", 8666);
    clnt.Connect("192.168.1.64", 8666);
    //for test we use 
    char in_case = 0;
    
    std::string tmp_index = "", index = "127.0.0.1:8666";
    std::string data = "";
    auto it = clnt._connections.begin();

    while(in_case != '4')
    {
        in_case = '0';
        printf("1 - List of connections \n");
        printf("2 - Select connection \n");
        printf("3 - Send data \n");
        printf("4 - exit \n");
        std::cin >> in_case;

        switch(in_case)
        {
            case '1':
                printf("List of connections \n");
                it = clnt._connections.begin();
        
                for( ; it != clnt._connections.end(); it++)
                {
                    PRINT_DBG("con %s \n", it->first.c_str());
                }

                break;

            case '2':
                printf("Enter connection index \n");
                std::cin >> tmp_index;
                if(clnt._connections.find(tmp_index) != clnt._connections.end())
                {
                    index = tmp_index;
                    tmp_index = "";
                }
                break;

            case '3':
                    while(data != "exit")
                    {
                        if(clnt._connections[index]->Get_send_buffer_ptr() == nullptr)
                        {
                            printf("Send data to server(type 'exit' to return previous menu): \n");
                            std::cin >>  data;

                            if(clnt._connections.find(index) != clnt._connections.end())
                            {
                                cross_socket::Buffer send_buff = {(char*)data.c_str(), static_cast<uint32_t>(data.size())};
                                clnt._connections[index]->Set_send_buffer_ptr(&send_buff);
                            }
                            else
                            {
                                PRINT_DBG("connection is not found \n");
                                break;
                            }
                        }
                    }
                    
                    data = "";

                break;

        }
    }
    
    return 0;
}