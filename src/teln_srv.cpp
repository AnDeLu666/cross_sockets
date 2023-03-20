#include "cross_socket_srv.h"

#include <string>
#include <chrono>



int main(int argc, char const *argv[])
{
    const char *login = "login\n";

    std::string user_pass = "root:1";

    cross_socket::CrossSocketSrv srv(8666);

    srv.Start();
    int dbg_k = srv.GetConn_s();
    printf("server started %d\n", dbg_k);

    while(srv.GetStatus() == cross_socket::Status::EMPTY)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    if(srv.GetStatus() == cross_socket::Status::ACCEPTED)
    {
        srv.Recv();

        srv.Send(login);
    }

    return 0;
}