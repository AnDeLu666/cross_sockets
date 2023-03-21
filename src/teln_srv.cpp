#include "cross_socket_srv.h"

#include <string>
#include <chrono>


int main(int argc, char const *argv[])
{
    std::string login = "login\n";

    std::string user_pass = "root:1";

    cross_socket::CrossSocketSrv srv(8666);

    srv.Start();

    while(srv.GetStatus() == cross_socket::Status::EMPTY)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    if(srv.GetStatus() == cross_socket::Status::ACCEPTED)
    {
        while (srv.GetBuffer() != "exit")
        {
            srv.Recv();

            printf("get buffer %s\n", srv.GetBuffer());

            srv.Send(login.c_str());
        }
    }

    return 0;
}