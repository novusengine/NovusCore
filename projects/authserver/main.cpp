#include "AuthSocketHandler.h"


int main()
{
    asio::io_service io_service;
    AuthSocketHandler server(io_service, 3724);
    server.Init();
    server.Start();

    std::thread run_thread([&]
    {
        io_service.run();
    });

    printf("Authserver Running\n\n");
    std::getchar();

    return 0;
}
