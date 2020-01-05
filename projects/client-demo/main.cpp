#include <Utils/DebugHandler.h>
#include <Utils/StringUtils.h>

#include "ClientHandler.h"
#include "ConsoleCommands.h"
#include <Utils/Message.h>
#include <Networking/Connection.h>

#ifdef _WIN32
#include <Windows.h>
#endif
#include <future>

//The name of the console window.
#define WINDOWNAME "Client"

i32 main()
{
    /* Set up console window title */
#ifdef _WIN32 //Windows
    SetConsoleTitle(WINDOWNAME);
#endif

    NC_LOG_SUCCESS("Client Started");

    asio::io_service io_service(2);
    std::thread run_thread([&]
    {
        asio::io_service::work work(io_service);
        io_service.run();
    });

    ClientHandler clientHandler;
    clientHandler.Start();

    // This has to run after clientHandler.Start()
    Connection connection(new asio::ip::tcp::socket(io_service));
    connection.Start();

    ConsoleCommandHandler consoleCommandHandler;
    auto future = std::async(std::launch::async, StringUtils::GetLineFromCin);
    while (true)
    {
        Message message;
        bool shouldExit = false;
        while (clientHandler.TryGetMessage(message))
        {
            if (message.code == MSG_OUT_EXIT_CONFIRM)
            {
                shouldExit = true;
                break;
            }
            else if (message.code == MSG_OUT_PRINT)
            {
                NC_LOG_MESSAGE(*message.message);
                delete message.message;
            }
            else if (message.code == MSG_OUT_SETUP_COMPLETE)
            {
                // Client is setup and is ready to start networking
                //connection.Start();
            }
        }

        if (shouldExit)
            break;

        if (future.wait_for(std::chrono::milliseconds(50)) == std::future_status::ready)
        {
            std::string command = future.get();
            std::transform(command.begin(), command.end(), command.begin(), ::tolower); // Convert command to lowercase

            consoleCommandHandler.HandleCommand(clientHandler, command);
            future = std::async(std::launch::async, StringUtils::GetLineFromCin);
        }
    }

    return 0;
}
