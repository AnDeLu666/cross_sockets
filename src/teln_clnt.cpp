#include "cross_socket_clnt.h"

#include <string>
#include <iostream>

int main(int argc, char const* argv[]) 
{   
    std::string data = "";
    
    cross_socket::CrossSocketClnt clnt;

    printf("enter command : \n");

    clnt.Connect(8666);
    
    while(data != "exit")
    {
       std::getline(std::cin, data);

       clnt.ConnectionHandler(data.c_str());

    }

    return 0;
}