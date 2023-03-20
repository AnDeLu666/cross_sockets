#include "cross_socket_clnt.h"

#include <string>

int main(int argc, char const* argv[]) 
{   
    std::string msg = "Hello from client";
    
    cross_socket::CrossSocketClnt clnt(8666);

    printf("client started");

    clnt.Connect();

    clnt.Send(msg.c_str());

    clnt.Recv();

    return 0;
}