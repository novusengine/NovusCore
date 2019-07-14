#include <Utils/DebugHandler.h>
#include <Utils/StringUtils.h>
#include <Config/ConfigHandler.h>
#include <Database/DatabaseConnector.h>

#include "ConnectionHandlers/AuthConnectionHandler.h"

#include "AuthServerHandler.h"
#include "ConsoleCommands.h"
#include "Message.h"

#ifdef _WIN32
#include <Windows.h>
#endif

//The name of the console window.
#define WINDOWNAME "Authentication Server"

i32 main()
{
    /* Set up console window title */
#ifdef _WIN32 //Windows
    SetConsoleTitle(WINDOWNAME);
#endif

    /* Load Database Config Handler */
    if (!ConfigHandler::Load("database.json"))
    {
        std::cin.ignore();
        return 0;
    }

    /* Load Database Information here */
    DatabaseConnectionDetails dbConnections[DATABASE_TYPE::COUNT];
    dbConnections[DATABASE_TYPE::AUTHSERVER] = DatabaseConnectionDetails(ConfigHandler::GetJsonObjectByKey("auth_database"));

    /* Pass DatabaseConnectionDetails to 'Setup' */
    DatabaseConnector::Setup(dbConnections);

    /* Load Config Handler */
    if (!ConfigHandler::Load("authserver.json"))
    {
        std::cin.ignore();
        return 0;
    }

    asio::io_service ioService(2);
    AuthConnectionHandler authConnectionHandler(ioService, ConfigHandler::GetOption<u16>("port", 3724));
    authConnectionHandler.Start();

    AuthServerHandler authServerHandler;
    authServerHandler.Start();

    srand(static_cast<u32>(time(nullptr)));
    std::thread run_thread([&] {
        ioService.run();
    });

    ConsoleCommandHandler consoleCommandHandler;
    auto future = std::async(std::launch::async, StringUtils::GetLineFromCin);
    while (true)
    {
        Message message;
        bool shouldExit = false;
        while (authServerHandler.TryGetMessage(message))
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
        }

        if (shouldExit)
            break;

        if (future.wait_for(std::chrono::milliseconds(50)) == std::future_status::ready)
        {
            std::string command = future.get();
            std::transform(command.begin(), command.end(), command.begin(), ::tolower); // Convert command to lowercase

            consoleCommandHandler.HandleCommand(authServerHandler, command);
            future = std::async(std::launch::async, StringUtils::GetLineFromCin);
        }
    }

    NC_LOG_SUCCESS("Authserver running on port: %u", authConnectionHandler.GetPort());

    std::string message = "--- Thank you for flying with NovusCore, press enter to exit --- ";
    for (i32 i = 0; i < message.size() - 1; i++)
        std::cout << "-";
    std::cout << std::endl
              << message << std::endl;
    for (i32 i = 0; i < message.size() - 1; i++)
        std::cout << "-";
    std::cout << std::endl;

    ioService.stop();
    while (!ioService.stopped())
    {
        std::this_thread::yield();
    }

    authConnectionHandler.Stop();
    DatabaseConnector::Stop();
    return 0;
}
