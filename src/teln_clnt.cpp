#include "cross_socket_clnt.h"

#include <string>
#include <iostream>

int main(int argc, char const* argv[]) 
{   
    std::string data = "";
    
    cross_socket::CrossSocketClnt clnt;

    printf("client started");

    clnt.Connect(8666);

    printf("send data  %s\n", data.c_str());
    
    while(data != "exit")
    {
       std::getline(std::cin, data);

       clnt.Send(data.c_str()); 
       
       clnt.Recv();
    }

    return 0;
}